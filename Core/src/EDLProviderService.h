#ifndef CORE_SRC_EDLPROVIDERSERVICE_H
#define CORE_SRC_EDLPROVIDERSERVICE_H

#include <QtServiceBase>
#include <QCoreApplication>

#include "EDLProviderServer.h"

namespace edlprovider
{
namespace core
{

class EDLProviderService : public QtService<QCoreApplication>
{
public:
    EDLProviderService(int argc, char** argv);

    ~EDLProviderService();

    bool setUp();

protected:
    virtual void start() override final;

    virtual void stop() override final;

    virtual void createApplication(int &argc, char **argv) override final;

    virtual int executeApplication() override final;

private:
    QScopedPointer<soap::EDLProviderServer> soapServer_;
};

}
}

#endif // CORE_SRC_EDLPROVIDERSERVICE_H
