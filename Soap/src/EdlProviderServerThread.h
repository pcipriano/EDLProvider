#ifndef SOAP_SRC_EDLPROVIDERSERVERTHREAD_H
#define SOAP_SRC_EDLPROVIDERSERVERTHREAD_H

#include <QThread>

namespace edlprovider
{
namespace soap
{

/*!
 * \brief The EdlProviderServerThread class is simply a helper to launch the Edl provider soap server in a separate thread.
 */
class EdlProviderServerThread : public QThread
{
public:
    /*!
     * \brief EdlProviderServerThread constructor.
     */
    EdlProviderServerThread();
};

}
}

#endif // SOAP_SRC_EDLPROVIDERSERVERTHREAD_H
