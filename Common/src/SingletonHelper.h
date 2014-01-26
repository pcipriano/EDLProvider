#ifndef COMMON_SRC_SINGLETONHELPER_H
#define COMMON_SRC_SINGLETONHELPER_H

#include "Uncopyable.h"

namespace common
{
namespace util
{

template<typename T>
class SingletonHelper : private Uncopyable
{
public:
    static T& instance();
};

}
}

#include "SingletonHelperImpl.h"

#endif // COMMON_SRC_SINGLETONHELPER_H
