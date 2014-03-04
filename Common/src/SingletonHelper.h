#ifndef COMMON_SRC_SINGLETONHELPER_H
#define COMMON_SRC_SINGLETONHELPER_H

#include "Uncopyable.h"

namespace common
{
namespace util
{

/*!
 * \brief Helper class to provide singletons without adding boiler plate code to a class.
 */
template<typename T>
class SingletonHelper : private Uncopyable
{
public:
    /*!
     * \brief Gives access to the singleton instance object.
     * \return The singleton instance.
     */
    static T& instance();
};

}
}

#include "SingletonHelperImpl.h"

#endif // COMMON_SRC_SINGLETONHELPER_H
