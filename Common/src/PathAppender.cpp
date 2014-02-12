#include "PathAppender.h"

#include <QDir>

using namespace common::util;

QString PathAppender::combine(const QString& path1, const QString& path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

PathAppender::PathAppender(const QString& path)
    : path_(path)
{
}

PathAppender::PathAppender(const QString& path1, const QString path2)
    : path_(combine(path1, path2))
{
}

PathAppender& PathAppender::operator<<(const QString& path)
{
    path_ = combine(path_, path);
    return *this;
}
