#ifndef SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
#define SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H

#include <QRunnable>
#include <QScopedPointer>

#include "EdlProviderServer.h"

namespace edlprovider
{
namespace soap
{

/*!
 * \brief The EdlProviderProcessRequest class is where soap requests are dispatched and processed.
 */
class EdlProviderProcessRequest : public QRunnable
{
public:
    /*!
     * \brief EdlProviderProcessRequest constructor.
     * \param edlProvider The soap server instance to use.
     */
    explicit EdlProviderProcessRequest(EdlProviderBindingService* const edlProvider);

    /*!
     * \brief Processed the soap request in a separate thread.
     */
    void run() override final;

private:
    QScopedPointer<EdlProviderBindingService> edlProvider_;     //!< Soap server instance.
};

}
}

#endif // SOAP_SRC_EDLPROVIDERPROCESSREQUEST_H
