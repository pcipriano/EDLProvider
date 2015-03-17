#ifndef PLUGINS_INTERFACES_SRC_EDLEXCEPTION_H
#define PLUGINS_INTERFACES_SRC_EDLEXCEPTION_H

#include <string>
#include <cstdint>
#include <exception>

#include <QtGlobal>

namespace plugins
{
namespace interfaces
{

/*!
 * \brief The EdlException class is a custom exception to use in EDL implementations to throw when exceptional situations occur.
 *
 *  The exception is processed on the server to send back to the client a soap fault with detailed information about the error.
 */
class EdlException : public std::exception
{
public:
    /*!
     * \brief The EdlError list of common error types.
     */
    enum class EdlError : uint8_t
    {
        //!< Generic exception that does not fall into any specific category.
        GENERIC,
        //!< Error when a missing required value is not present in the request.
        REQUIRED_VALUE,
        //!< If markin is bigger than markout.
        MARKIN_BIGGER_THAN_MARKOUT,
        //!< Error when the specified markein or markout is outside of the clip total duration.
        MARK_INOUT_OUTSIDE_DURATION,
        //!< The frame rate specified is not supported.
        UNSUPPORTED_FRAME_RATE,
    };

    /*!
     * \brief EdlException constructor.
     * \param errorType The error type of the excpetion.
     */
    explicit EdlException(const EdlError errorType);

    /*!
     * \overload
     * \param detailInfo Detailed information about the exception.
     */
    EdlException(const EdlError errorType, std::string detailInfo);

    /*!
     * \brief The EdlError destructor.
     */
    ~EdlException() { }

    /*!
     * \brief Returns information about the exception.
     * \return Text giving generic details about the exception.
     */
    virtual const char* what() const Q_DECL_NOEXCEPT final override;

    /*!
     * \brief Gets detailed information about the exception.
     * \return Text giving detailed details about the exception.
     */
    const char* detailedInformation() const Q_DECL_NOEXCEPT { return detailedInfo_.c_str(); }

    /*!
     * \brief Gets the error code of the exception.
     * \return The error code detailing the cause of the exception.
     */
    EdlError getCode() const Q_DECL_NOEXCEPT { return  errorType_; }

private:
    const EdlError errorType_;          //!< The error code of the exception.

    const std::string detailedInfo_;    //!< The detailed information of the exception.
};

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLEXCEPTION_H
