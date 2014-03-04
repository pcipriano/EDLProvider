#ifndef COMMON_SRC_PATHAPPENDER_H
#define COMMON_SRC_PATHAPPENDER_H

#include <QString>

namespace common
{
namespace util
{

/*!
 * \brief The PathAppender class is a helper utility to concatenate path strings.
 */
class PathAppender
{
public:
    /*!
     * \brief Combines two path string to generate a valid path string.
     * \param path1 The first path that gets appended.
     * \param path2 The second path that is appended to first one.
     * \return A \c QString containing the two paths combined.
     */
    static QString combine(const QString& path1, const QString& path2);

    /*!
     * \brief PathAppender constructor.
     * \param path The initial path string the object will hold.
     */
    explicit PathAppender(const QString& path);

    /*!
     * \overload
     * \param path1 The first path that gets appended.
     * \param path2 The second path that is appended to first one.
     */
    PathAppender(const QString& path1, const QString path2);

    /*!
     * \brief Convenient operator to append multiple path strings.
     * \param path The path string to append.
     * \return The instance of \c PathAppender where the pats are being appende to.
     */
    PathAppender& operator<<(const QString& path);

    /*!
     * \brief Returns the full path string the object is holding.
     * \return The \c QString path.
     */
    QString toString() const { return path_; }

    /*!
     * \brief Implicit conversion to \c QString.
     */
    operator QString() const { return path_; }

private:
    QString path_;      //!< Path string.
};

}
}

#endif // COMMON_SRC_PATHAPPENDER_H
