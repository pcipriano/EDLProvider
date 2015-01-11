#ifndef CORE_SRC_EDLPROVIDERSERVICE_H
#define CORE_SRC_EDLPROVIDERSERVICE_H

#include <QtServiceBase>
#include <QCoreApplication>

#include "Configuration.h"

namespace edlprovider
{
namespace soap
{
class EdlProviderServer;
class EdlProviderServerThread;
}
}

namespace edlprovider
{
namespace core
{

/*!
 * \brief The EdlProviderService class is the main entry for setting up everything and starting the soap service.
 */
class EdlProviderService : public QtService<QCoreApplication>
{
public:
    /*!
     * \brief EdlProviderService constructor.
     * \param argc Arguments count passed when initializing the service.
     * \param argv Arguments values.
     */
    EdlProviderService(int argc, char** argv);

    /*!
     * \brief EdlProviderService destructor.
     */
    ~EdlProviderService();

protected:
    /*!
    * \sa {QtService::start}
    */
    virtual void start() override final;

    /*!
    * \sa {QtService::stop}
    */
    virtual void stop() override final;

    /*!
    * \sa {QtService::createApplication}
    */
    virtual void createApplication(int &argc, char **argv) override final;

    /*!
    * \sa {QtService::executeApplication}
    */
    virtual int executeApplication() override final;

private:
    /*!
     * \brief Sets up settings for the correct functioning of the service.
     * \return True if successful, false otherwise.
     */
    bool setUp();

    QScopedPointer<Configuration> configuration_;   //!< Application configuration manager.

    QScopedPointer<soap::EdlProviderServer> soapServer_;    //!< Soap server handler.

    QScopedPointer<soap::EdlProviderServerThread> soapServerThread_;    //!< Soap server thread object.
};

}
}

#endif // CORE_SRC_EDLPROVIDERSERVICE_H
