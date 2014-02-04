#ifndef COMMON_SRC_PATHAPPENDER_H
#define COMMON_SRC_PATHAPPENDER_H

#include <QString>

namespace common
{
namespace util
{

class PathAppender
{
public:
    static QString combine(const QString& path1, const QString& path2);

    explicit PathAppender(const QString& path);

    PathAppender(const QString& path1, const QString path2);

    PathAppender& operator<<(const QString& path);

    PathAppender& operator+(const QString& path);

    QString toString() const { return path_; }

    operator QString() const { return path_; }

private:
    QString path_;
};

}
}

#endif // COMMON_SRC_PATHAPPENDER_H
