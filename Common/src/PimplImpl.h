#ifndef COMMON_SRC_PIMPLIMPL_H
#define COMMON_SRC_PIMPLIMPL_H

#include <utility>

namespace common
{
namespace util
{

template<typename T>
Pimpl<T>::Pimpl()
    : impl_(new T())
{
}

template<typename T>
template<typename ...Args>
Pimpl<T>::Pimpl(Args&& ...args)
    : impl_(new T(std::forward<Args>(args)...))
{
}

template<typename T>
Pimpl<T>::Pimpl(const Pimpl& other)
    : impl_(new T(*other.impl_.get()))
{
}

template<typename T>
Pimpl<T>::~Pimpl()
{
}

template<typename T>
Pimpl<T>& Pimpl<T>::operator=(const Pimpl& other)
{
    if (this == &other)
        return *this;

    Pimpl<T> temp(other);
    swap(temp);
    return *this;
}

template<typename T>
void Pimpl<T>::swap(Pimpl& other)
{
    impl_.swap(other.impl_);
}

template<typename T>
T* Pimpl<T>::operator->()
{
    return impl_.get();
}

template<typename T>
const T* const Pimpl<T>::operator->() const
{
    return impl_.get();
}

template<typename T>
T& Pimpl<T>::operator*()
{
    return *impl_.get();
}

template<typename T>
const T& Pimpl<T>::operator*() const
{
    return *impl_.get();
}

}
}

#endif // COMMON_SRC_PIMPLIMPL_H
