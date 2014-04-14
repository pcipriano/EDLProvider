#ifndef SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
#define SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H

#include <QRunnable>
#include <QScopedPointer>

#include "EDLProviderServer.h"

namespace edlprovider
{
namespace soap
{

class EDLProviderProcessRequest : public QRunnable
{
public:
    explicit EDLProviderProcessRequest(EdlProviderBindingService* const edlProvider);

    void run() override final;

private:
    QScopedPointer<EdlProviderBindingService> edlProvider_;
};

}
}

#endif // SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
