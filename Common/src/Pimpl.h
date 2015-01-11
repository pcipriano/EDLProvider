#ifndef COMMON_SRC_PIMPL_H
#define COMMON_SRC_PIMPL_H

#include <memory>

namespace common
{
namespace util
{

/*!
 * \brief The Pimpl class is a helper class to ease the use of the pimpl idiom.
 *
 *  The idea is to use it like this:
 *  \code
 *  class MyClass
 *  {
 *  public:
 *      void func();
 *
 *  private:
 *      //Can be a struct or a class doesn't matter
 *      struct Impl;
 *      common::util::Pimpl<Impl> impl_;
 *  }
 *  \endcode
 */
template<typename T>
class Pimpl
{
public:
    /*!
     * \brief Pimpl constructor.
     */
    Pimpl();

    /*!
     * \brief Pimpl constructor with arguments.
     */
    template<typename ...Args>
    Pimpl(Args&& ...);

    /*!
     * \brief Pimpl copy constructor.
     * \param other The other instance to copy from.
     */
    Pimpl(const Pimpl& other);

    /*!
     * \brief Pimpl destructor.
     */
    ~Pimpl();

    /*!
     * \brief assignment operator.
     * \param other The other instance to assign from.
     * \return This object instance reference.
     */
    Pimpl& operator=(const Pimpl& other);

    /*!
     * \brief Swap function.
     * \param other The other instance to swap with.
     */
    void swap(Pimpl& other);

    /*!
     * \brief Non-const dereference operator.
     * \return Stored pointer to implementation.
     */
    T* operator->();

    /*!
     * \brief Const dereference operator.
     * \return Stored pointer to implementation with const access.
     */
    const T* const operator->() const;

    /*!
     * \brief Return a reference to the underlying implementation.
     * \return The implementation object stored.
     */
    T& operator*();

    /*!
     * \brief Return a reference to the underlying implementation with const access.
     * \return The implementation object stored.
     */
    const T& operator*() const;

private:
    std::unique_ptr<T> impl_;       //!< Implementation pointer managed by a smart pointer.
};

}
}

#include "PimplImpl.h"

#endif // COMMON_SRC_PIMPL_H
