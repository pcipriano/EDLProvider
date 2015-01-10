#ifndef EDLPROVIDERSERVERTHREAD_H
#define EDLPROVIDERSERVERTHREAD_H

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

#endif // EDLPROVIDERSERVERTHREAD_H
