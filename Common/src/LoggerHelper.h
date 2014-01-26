#ifndef COMMON_SRC_LOGGERHELPER_H
#define COMMON_SRC_LOGGERHELPER_H

#include <QString>

#include "Pimpl.h"
#include "SingletonHelper.h"

namespace common
{
namespace logging
{

class LoggerHelper : public util::SingletonHelper<LoggerHelper>
{
public:
    void setLoggingFile(const QString& filePath);

    void handleLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    LoggerHelper() = default;

    friend class util::SingletonHelper<LoggerHelper>;

    struct Impl;
    util::Pimpl<Impl> impl_;
};

}
}

#endif // COMMON_SRC_LOGGERHELPER_H
