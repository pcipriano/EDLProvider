#ifndef COMMON_SRC_SINGLETONHELPERIMPL_H
#define COMMON_SRC_SINGLETONHELPERIMPL_H

namespace common
{
namespace util
{

template<typename T>
T& SingletonHelper<T>::instance()
{
    static T instance;

    return instance;
}

}
}

#endif // COMMON_SRC_SINGLETONHELPERIMPL_H
