#include "EdlProviderServer.h"

#include <QThreadPool>
#include <QCoreApplication>

#include "easylogging++.h"

#include "PathAppender.h"
#include "EdlProviderBinding.nsmap"
#include "EdlProviderProcessRequest.h"

using namespace edlprovider::soap;
using namespace plugins::interfaces;

const char* const EDL_PLUGIN_FOLDER = "plugins";

/*!
 * \brief Sets the edit rate from a rounded value.
 * \param editRate The rounded value of the edit rate.
 * \param editRateResult The edit rate object to set.
 */
static bool setEditRate(double editRate, fims__RationalType& editRateResult)
{
    if (editRate == 24.000)
    {
        editRateResult.__item = 24;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate == 25.000)
    {
        editRateResult.__item = 25;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate == 30.000)
    {
        editRateResult.__item = 30;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate == 48.000)
    {
        editRateResult.__item = 48;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate == 50.000)
    {
        editRateResult.__item = 50;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate == 60.000)
    {
        editRateResult.__item = 60;
        editRateResult.numerator = L"1";
        editRateResult.denominator = L"1";
    }
    else if (editRate > 23.000 && editRate < 24.000)
    {
        editRateResult.__item = 24;
        editRateResult.numerator = L"1000";
        editRateResult.denominator = L"1001";
    }
    else if (editRate > 29.000 && editRate < 30.000)
    {
        editRateResult.__item = 30;
        editRateResult.numerator = L"1000";
        editRateResult.denominator = L"1001";
    }
    else if (editRate > 59.000 && editRate < 60.000)
    {
        editRateResult.__item = 60;
        editRateResult.numerator = L"1000";
        editRateResult.denominator = L"1001";
    }
    else
    {
        return false;
    }

    return true;
}

EdlProviderServer::EdlProviderServer(const QString& host, bool pluginsAutoReload)
    : pluginManager_(new EdlPluginManager(common::util::PathAppender::combine(QCoreApplication::applicationDirPath(), ::EDL_PLUGIN_FOLDER),
                                          pluginsAutoReload)),
      hostName_(host)
{
}

int EdlProviderServer::run(int port)
{
    if (soap_valid_socket(this->master) || soap_valid_socket(bind(hostName_.isEmpty() ? NULL : hostName_.toLocal8Bit().data(), port, 100)))
    {
        std::string hostName = hostName_.isEmpty() ? "localhost" : hostName_.toStdString();
        LOG(INFO) << "Running server in [" << hostName << ":" << std::to_string(port) << "].";

        Q_FOREVER
        {
            if (!soap_valid_socket(accept()))
                return this->error;

            EdlProviderProcessRequest* processor = new EdlProviderProcessRequest(copy());
            QThreadPool::globalInstance()->start(processor);
        }
    }
    else
        return this->error;

    return SOAP_OK;
}

EdlProviderBindingService* edlprovider::soap::EdlProviderServer::copy()
{
    return new EdlProviderServer(*this);
}

int EdlProviderServer::getInstalledEdls(edlprovider__InstalledEdlsResponseType& edlprovider__installedEdlsResponse)
{
    //Get all plugin names.
    _edlprovider__Map* edls = soap_new_req__edlprovider__Map(edlprovider__installedEdlsResponse.soap);
    for (EdlInterface* const edl : pluginManager_->getEdls())
    {
        std::wstring name = edl->getEdlName();
        std::wstring extension = edl->getEdlExtension();
        auto item = soap_new_req_edlprovider__MapItemType(edlprovider__installedEdlsResponse.soap,
                                                          name,
                                                          extension);
        edls->item.push_back(item);
    }

    edlprovider__installedEdlsResponse.edlprovider__Map = edls;

    return SOAP_OK;
}

int EdlProviderServer::getEdl(edlprovider__EdlCreateRequestType* edlprovider__getEdlRequest,
                              edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse)
{
    return processGetEdl(edlprovider__getEdlRequest->soap,
                         edlprovider__getEdlRequest->edlType,
                         edlprovider__getEdlRequest->edlSequenceName,
                         edlprovider__getEdlRequest->edlFramesPerSecond,
                         edlprovider__getEdlRequest->clips->clips,
                         edlprovider__getEdlResponse);
}

int EdlProviderServer::getEdlDouble(edlprovider__EdlCreateRequestDoubleType* edlprovider__getEdlDoubleRequest,
                                    edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse)
{
    auto frameRate = soap_new_fims__RationalType(edlprovider__getEdlDoubleRequest->soap);
    if (!setEditRate(edlprovider__getEdlDoubleRequest->edlFramesPerSecond, *frameRate))
    {
        LOG(ERROR) << "Specified framerate [" << edlprovider__getEdlDoubleRequest->edlFramesPerSecond << "] not supported.";
        buildEdlSoapFault(edlprovider__getEdlDoubleRequest->soap,
                          "Framerate not supported.",
                          edlprovider__EdlProviderErrorCodeType__EXT_USCORES00_USCORE0002,
                          QString("Specified [%1] framerate not supported by the application").arg(edlprovider__getEdlDoubleRequest->edlFramesPerSecond));
        return SOAP_FAULT;
    }

    return processGetEdl(edlprovider__getEdlDoubleRequest->soap,
                         edlprovider__getEdlDoubleRequest->edlType,
                         edlprovider__getEdlDoubleRequest->edlSequenceName,
                         frameRate,
                         edlprovider__getEdlDoubleRequest->clips->clips,
                         edlprovider__getEdlResponse);
}

soap_int32 EdlProviderServer::processGetEdl(soap* const soap,
                                            const std::wstring& edlType,
                                            const std::wstring* const edlSequenceName,
                                            const fims__RationalType* const edlFrameRate,
                                            const std::vector<edlprovider__ClipType*>& clips,
                                            edlprovider__EdlCreateResponseType& response)
{
    EdlInterface* edl = pluginManager_->findEdl(edlType);

    if (edl == nullptr)
    {
        buildEdlSoapFault(soap,
                          "Edl type not found.",
                          edlprovider__EdlProviderErrorCodeType__EXT_USCORES00_USCORE0001,
                          "Specified Edl type for generating an Edl not available.");
        return SOAP_FAULT;
    }
    else
    {
        try
        {
            QByteArray edlData = edl->createEdl(edlSequenceName,
                                                edlFrameRate,
                                                clips);

            auto resultData = soap_new_xsd__base64Binary(soap);
            char* result = (char*) soap_malloc(soap, edlData.size());
            memcpy(result, edlData.constData(), edlData.size());
            resultData->__ptr = (unsigned char*) result;
            resultData->__size = edlData.size();
            response.getEdlResult = resultData;
        }
        catch (const std::invalid_argument& ia)
        {
            buildSoapFault(soap,
                           "Invalid request parameters.",
                           fims__ErrorCodeType__DAT_USCORES00_USCORE0006,
                           std::vector<fims__InnerFaultType*>(),
                           ia.what());
            return SOAP_FAULT;
        }
        catch (...)
        {
            buildSoapFault(soap,
                           "Unknown error.",
                           fims__ErrorCodeType__INF_USCORES00_USCORE0003,
                           std::vector<fims__InnerFaultType*>(),
                           "Unknow error writing Edl.");
            return SOAP_FAULT;
        }
    }

    return SOAP_OK;
}

void EdlProviderServer::buildEdlSoapFault(soap* const soap,
                                          const QString& description,
                                          edlprovider__EdlProviderErrorCodeType extendedCode,
                                          const QString& detailInfo, std::vector<fims__InnerFaultType*> innerFault,
                                          bool isSenderFault)
{
    buildSoapFault(soap,
                   description,
                   fims__ErrorCodeType__EXT_USCORES00_USCORE0000,
                   innerFault,
                   detailInfo,
                   &extendedCode,
                   isSenderFault);
}

void EdlProviderServer::buildSoapFault(soap* const soap,
                                       const QString& description,
                                       fims__ErrorCodeType mainErrorCode,
                                       std::vector<fims__InnerFaultType*> innerFault,
                                       const QString& detailInfo,
                                       edlprovider__EdlProviderErrorCodeType* extendedCode,
                                       bool isSenderFault)
{
    std::wstring* descriptionTemp = soap_new_set_std__wstring(soap);
    descriptionTemp->assign(description.toStdWString());

    auto fault = soap_new_edlprovider__EdlProviderFaultType(soap);
    fault->description = descriptionTemp;
    fault->code = mainErrorCode;
    fault->innerFault = innerFault;

    if (extendedCode != nullptr)
    {
        auto errorCode = (edlprovider__EdlProviderErrorCodeType*) soap_malloc(soap, sizeof(edlprovider__EdlProviderErrorCodeType));
        *errorCode = *extendedCode;
        fault->extendedCode = errorCode;
    }

    if (!detailInfo.isEmpty())
    {
        std::wstring* detailTemp = soap_new_set_std__wstring(soap);
        detailTemp->assign(detailInfo.toStdWString());
        fault->detail = detailTemp;
    }

    if (isSenderFault)
        soap_senderfault("An error occurred from the client, see detail", NULL);
    else
        soap_receiverfault("An error occurred on the server, see detail", NULL);

    //Check if SOAP 1.2 is used
    if (soap->version == 2)
        soap->fault->SOAP_ENV__Detail = soap_new_req_SOAP_ENV__Detail(soap, fault->soap_type(), fault);
    else
        soap->fault->detail = soap_new_req_SOAP_ENV__Detail(soap, fault->soap_type(), fault);
}
