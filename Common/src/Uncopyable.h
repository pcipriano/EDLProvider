#ifndef COMMON_SRC_UNCOPYABLE_H
#define COMMON_SRC_UNCOPYABLE_H

namespace common
{
namespace util
{

/*!
 * \brief Helper class for marking other classes as uncopyable.
 */
class Uncopyable
{
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

}
}

#endif // COMMON_SRC_UNCOPYABLE_H
