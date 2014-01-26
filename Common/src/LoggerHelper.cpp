#include "LoggerHelper.h"

#include <QFile>
#include <QtGlobal>
#include <QDateTime>
#include <QTextStream>

using namespace common::logging;

void logMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    LoggerHelper::instance().handleLogMessage(type, context, msg);
}

struct LoggerHelper::Impl
{
    QString filePath_;

    Impl()
    {
        qInstallMessageHandler(logMessage);
    }

    void handleLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        QFile logFile(filePath_);
        if (logFile.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream streamer(&logFile);
            switch (type)
            {
                case QtDebugMsg:
                    streamer << QDateTime::currentDateTime().toString() <<" Warning: " << msg <<  "\n";
                    break;
                case QtWarningMsg:
                    streamer << QDateTime::currentDateTime().toString() <<" Warning: " << msg <<  "\n";
                    break;
                case QtCriticalMsg:
                    streamer << QDateTime::currentDateTime().toString() <<" Critical: " << msg <<  "\n";
                    break;
                case QtFatalMsg:
                    streamer << QDateTime::currentDateTime().toString() <<" Fatal: " << msg <<  "\n";
                    break;
            }
        }
    }
};

void LoggerHelper::setLoggingFile(const QString& filePath)
{
    impl_->filePath_ = filePath;
}

void LoggerHelper::handleLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    impl_->handleLogMessage(type, context, msg);
}
