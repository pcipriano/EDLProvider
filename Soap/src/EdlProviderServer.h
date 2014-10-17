#ifndef SOAP_SRC_EDLPROVIDERSERVER_H
#define SOAP_SRC_EDLPROVIDERSERVER_H

#include "EdlPluginManager.h"
#include "EDLProviderEdlProviderBindingService.h"

namespace edlprovider
{
namespace soap
{

class EdlProviderServer : public EdlProviderBindingService
{
public:
    explicit EdlProviderServer(const QString& host, bool pluginsAutoReload);

    int run(int port) override final;

private:
    EdlProviderBindingService* copy() override final;

    int getInstalledEdls(edlprovider__InstalledEdlsResponseType& edlprovider__installedEdlsResponse) override final;

    int getEdl(edlprovider__EdlCreateRequestType* edlprovider__getEdlRequest,
               edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse) override final;

    int getEdlDouble(edlprovider__EdlCreateRequestDoubleType* edlprovider__getEdlDoubleRequest,
                     edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse) override final;

    soap_int32 processGetEdl(soap* const soap,
                             const std::wstring& edlType,
                             const std::wstring* const edlSequenceName,
                             const fims__RationalType* const edlFrameRate,
                             const std::vector<edlprovider__ClipType*>& clips,
                             edlprovider__EdlCreateResponseType& response);

    void buildEdlSoapFault(soap* const soap,
                           const QString& description,
                           edlprovider__EdlProviderErrorCodeType extendedCode,
                           const QString& detailInfo = QString(),
                           std::vector<class fims__InnerFaultType*> innerFault = std::vector<class fims__InnerFaultType*>(),
                           bool isSenderFault = true);

    void buildSoapFault(soap* const soap,
                        const QString& description,
                        fims__ErrorCodeType mainErrorCode,
                        std::vector<class fims__InnerFaultType*> innerFault = std::vector<class fims__InnerFaultType*>(),
                        const QString& detailInfo = QString(),
                        edlprovider__EdlProviderErrorCodeType* extendedCode = nullptr,
                        bool isSenderFault = true);

    QSharedPointer<plugins::interfaces::EdlPluginManager> pluginManager_;

    QString hostName_;
};

}
}

#endif // SOAP_SRC_EDLPROVIDERSERVER_H
