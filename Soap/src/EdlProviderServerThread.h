#ifndef SOAP_SRC_EDLPROVIDERSERVERTHREAD_H
#define SOAP_SRC_EDLPROVIDERSERVERTHREAD_H

#include <QThread>

namespace edlprovider
{
namespace soap
{

class EdlProviderServerThread : public QThread
{
public:
    EdlProviderServerThread();
};

}
}

#endif // SOAP_SRC_EDLPROVIDERSERVERTHREAD_H
