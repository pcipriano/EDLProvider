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

class EdlProviderService : public QtService<QCoreApplication>
{
public:
    EdlProviderService(int argc, char** argv);

    ~EdlProviderService();

    bool setUp();

protected:
    virtual void start() override final;

    virtual void stop() override final;

    virtual void createApplication(int &argc, char **argv) override final;

    virtual int executeApplication() override final;

private:
    QScopedPointer<Configuration> configuration_;

    QScopedPointer<soap::EdlProviderServer> soapServer_;

    QScopedPointer<soap::EdlProviderServerThread> soapServerThread_;
};

}
}

#endif // CORE_SRC_EDLPROVIDERSERVICE_H
