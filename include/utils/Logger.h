#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QDebug>

namespace WSS {

class Logger {
public:
    static void initialize(const QString& logPath = "wss.log");
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);
    static void debug(const QString& message);
    
private:
    static void writeLog(const QString& level, const QString& message);
    static QString m_logPath;
    static QMutex m_mutex;
    static bool m_initialized;
};

inline void Logger::info(const QString& message) {
    writeLog("INFO", message);
    qDebug() << "[INFO]" << message;
}

inline void Logger::warning(const QString& message) {
    writeLog("WARNING", message);
    qWarning() << "[WARNING]" << message;
}

inline void Logger::error(const QString& message) {
    writeLog("ERROR", message);
    qCritical() << "[ERROR]" << message;
}

inline void Logger::debug(const QString& message) {
    #ifdef QT_DEBUG
    writeLog("DEBUG", message);
    #endif
}

} // namespace WSS
