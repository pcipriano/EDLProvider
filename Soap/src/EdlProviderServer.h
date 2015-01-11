#ifndef SOAP_SRC_EDLPROVIDERSERVER_H
#define SOAP_SRC_EDLPROVIDERSERVER_H

#include "EdlPluginManager.h"
#include "EDLProviderEdlProviderBindingService.h"

namespace edlprovider
{
namespace soap
{

/*!
 * \brief The EdlProviderServer class implements the soap service that serves requests to clients for producing EDL's.
 */
class EdlProviderServer : public EdlProviderBindingService
{
public:
    /*!
     * \brief EdlProviderServer constructor.
     * \param host The host name to use for the server.
     * \param pluginsAutoReload Flag to control the automatic load of the EDL plugins.
     */
    explicit EdlProviderServer(const QString& host, bool pluginsAutoReload);

    /*!
     * \brief Starts listening for requests.
     * \param port The port where to listen to.
     * \return \c SOAP_OK if service started correctly otherwise an error code.
     */
    int run(int port) override final;

private:
    /*!
     * \brief Creates a copy of the current instance of the server.
     * \return A copy of the server instance.
     */
    EdlProviderBindingService* copy() override final;

    /*!
     * \brief Gets the list of available EDL plugins.
     * \param edlprovider__installedEdlsResponse Response containing the available EDL plugins.
     * \return SOAP_OK if successful.
     */
    int getInstalledEdls(edlprovider__InstalledEdlsResponseType& edlprovider__installedEdlsResponse) override final;

    /*!
     * \brief Gets EDL data.
     * \param edlprovider__getEdlRequest Request parameters.
     * \param edlprovider__getEdlResponse Response containing EDL data.
     * \return SOAP_OK if successful.
     */
    int getEdl(edlprovider__EdlCreateRequestType* edlprovider__getEdlRequest,
               edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse) override final;

    /*!
     * \brief Gets EDL data using a \c double to specify the EDL frame rate.
     * \param edlprovider__getEdlDoubleRequest Request parameters.
     * \param edlprovider__getEdlResponse Response containing EDL data.
     * \return SOAP_OK if successful.
     */
    int getEdlDouble(edlprovider__EdlCreateRequestDoubleType* edlprovider__getEdlDoubleRequest,
                     edlprovider__EdlCreateResponseType& edlprovider__getEdlResponse) override final;

    /*!
     * \brief Processes an EDL request.
     * \param soap The soap object.
     * \param edlType The EDL type to use for generating the data.
     * \param edlSequenceName The EDL sequence name specified.
     * \param edlFrameRate The frame rate of the EDL.
     * \param clips All the clips to use for generating the EDL.
     * \param response Response containing EDL data.
     * \return SOAP_OK if successful.
     */
    soap_int32 processGetEdl(soap* const soap,
                             const std::wstring& edlType,
                             const std::wstring* const edlSequenceName,
                             const fims__RationalType* const edlFrameRate,
                             const std::vector<edlprovider__ClipType*>& clips,
                             edlprovider__EdlCreateResponseType& response);

    /*!
     * \brief Helper function to build the soap fault information.
     * \param soap The soap object.
     * \param description The description of the fault.
     * \param extendedCode The \c enum code for the fault.
     * \param detailInfo Any detailed information about the fault if available.
     * \param innerFault The inner fault that caused the error if available.
     * \param isSenderFault If \c true the error was caused because of the client request details, false if its a server error.
     */
    void buildEdlSoapFault(soap* const soap,
                           const QString& description,
                           edlprovider__EdlProviderErrorCodeType extendedCode,
                           const QString& detailInfo = QString(),
                           std::vector<class fims__InnerFaultType*> innerFault = std::vector<class fims__InnerFaultType*>(),
                           bool isSenderFault = true);

    /*!
     * \brief Helper function were the actual fault information is built.
     * \param soap The soap object.
     * \param description The description of the fault.
     * \param mainErrorCode The main error code, eg: \c fims__ErrorCodeType__EXT_USCORES00_USCORE0000.
     * \param innerFault The inner fault that caused the error if available.
     * \param detailInfo Any detailed information about the fault if available.
     * \param extendedCode The \c enum code for the fault.
     * \param isSenderFault If \c true the error was caused because of the client request details, false if its a server error.
     */
    void buildSoapFault(soap* const soap,
                        const QString& description,
                        fims__ErrorCodeType mainErrorCode,
                        std::vector<class fims__InnerFaultType*> innerFault = std::vector<class fims__InnerFaultType*>(),
                        const QString& detailInfo = QString(),
                        edlprovider__EdlProviderErrorCodeType* extendedCode = nullptr,
                        bool isSenderFault = true);

    QSharedPointer<plugins::interfaces::EdlPluginManager> pluginManager_;   //!< The manager that provides and loads EDL plugins.

    QString hostName_;  //!< The hostname to be used in the server.
};

}
}

#endif // SOAP_SRC_EDLPROVIDERSERVER_H
