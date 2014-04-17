#ifndef SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
#define SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H

#include <QRunnable>
#include <QScopedPointer>

#include "EdlProviderServer.h"

namespace edlprovider
{
namespace soap
{

class EdlProviderProcessRequest : public QRunnable
{
public:
    explicit EdlProviderProcessRequest(EdlProviderBindingService* const edlProvider);

    void run() override final;

private:
    QScopedPointer<EdlProviderBindingService> edlProvider_;
};

}
}

#endif // SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
