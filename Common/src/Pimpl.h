#ifndef COMMON_SRC_PIMPL_H
#define COMMON_SRC_PIMPL_H

#include <memory>

namespace common
{
namespace util
{

template<typename T>
class Pimpl
{
public:
    Pimpl();

    template<typename ...Args>
    Pimpl(Args&& ...);

    ~Pimpl();

    Pimpl(const T& other);

    Pimpl(const Pimpl& other);

    Pimpl& operator=(const Pimpl& other);

    void swap(Pimpl& other);

    T* operator->();

    const T* const operator->() const;

    T& operator*();

    const T& operator*() const;

private:
    std::unique_ptr<T> impl_;
};

}
}

#include "PimplImpl.h"

#endif // COMMON_SRC_PIMPL_H
