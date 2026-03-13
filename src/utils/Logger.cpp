#include "utils/Logger.h"

namespace WSS {

QString Logger::m_logPath = "wss.log";
QMutex Logger::m_mutex;
bool Logger::m_initialized = false;

void Logger::initialize(const QString& logPath) {
    m_logPath = logPath;
    m_initialized = true;
    info("日志系统初始化完成");
}

void Logger::writeLog(const QString& level, const QString& message) {
    if (!m_initialized) return;
    
    QMutexLocker locker(&m_mutex);
    
    QFile file(m_logPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        stream << timestamp << " [" << level << "] " << message << "\n";
        file.close();
    }
}

} // namespace WSS
