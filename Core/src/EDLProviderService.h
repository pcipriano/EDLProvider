#ifndef CORE_SRC_EDLPROVIDERSERVICE_H
#define CORE_SRC_EDLPROVIDERSERVICE_H

#include <QCoreApplication>

#include <QtServiceBase>

namespace edlprovider
{
namespace core
{

class EDLProviderService : public QtService<QCoreApplication>
{
public:
    EDLProviderService(int argc, char** argv);

    ~EDLProviderService();

protected:
    virtual void start() override;

    virtual void stop() override;

    virtual void createApplication(int &argc, char **argv) override;

    virtual int executeApplication() override;
};

}
}

#endif // CORE_SRC_EDLPROVIDERSERVICE_H
