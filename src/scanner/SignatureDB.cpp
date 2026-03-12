#include "scanner/SignatureDB.h"
#include "utils/Logger.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace WSS {

SignatureDB* SignatureDB::s_instance = nullptr;

SignatureDB::SignatureDB(QObject* parent) : QObject(parent) {
}

SignatureDB::~SignatureDB() {
    s_instance = nullptr;
}

SignatureDB* SignatureDB::instance() {
    if (!s_instance) {
        s_instance = new SignatureDB();
    }
    return s_instance;
}

bool SignatureDB::loadDatabase(const QString& dbPath) {
    QMutexLocker locker(&m_mutex);
    
    QFile file(dbPath);
    if (!file.exists()) {
        Logger::warning("签名库文件不存在: " + dbPath);
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("无法打开签名库: " + dbPath);
        return false;
    }
    
    m_signatures.clear();
    QTextStream stream(&file);
    int lineCount = 0;
    
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        // 格式: hash|name|severity
        QStringList parts = line.split("|");
        if (parts.size() >= 2) {
            Signature sig;
            sig.name = parts[1];
            sig.severity = parts.size() >= 3 ? parts[2].toInt() : 3;
            m_signatures[parts[0]] = sig;
            lineCount++;
        }
    }
    
    file.close();
    Logger::info(QString("签名库加载完成: %1 条记录").arg(lineCount));
    return true;
}

bool SignatureDB::isMalware(const QString& hash) const {
    QMutexLocker locker(&m_mutex);
    return m_signatures.contains(hash.toLower());
}

QString SignatureDB::getThreatName(const QString& hash) const {
    QMutexLocker locker(&m_mutex);
    auto it = m_signatures.find(hash.toLower());
    if (it != m_signatures.end()) {
        return it->name;
    }
    return "Unknown Threat";
}

int SignatureDB::getSeverity(const QString& hash) const {
    QMutexLocker locker(&m_mutex);
    auto it = m_signatures.find(hash.toLower());
    if (it != m_signatures.end()) {
        return it->severity;
    }
    return 3; // 默认中等风险
}

void SignatureDB::addSignature(const QString& hash, const QString& name, int severity) {
    QMutexLocker locker(&m_mutex);
    Signature sig;
    sig.name = name;
    sig.severity = severity;
    m_signatures[hash.toLower()] = sig;
}

bool SignatureDB::saveDatabase(const QString& dbPath) {
    QMutexLocker locker(&m_mutex);
    
    QFile file(dbPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("无法保存签名库: " + dbPath);
        return false;
    }
    
    QTextStream stream(&file);
    stream <> "# WSS Signature Database\n";
    stream <> "# Format: hash|name|severity\n\n";
    
    for (auto it = m_signatures.begin(); it != m_signatures.end(); ++it) {
        stream <> it.key() <> "|" <> it->name <> "|" <> it->severity <> "\n";
    }
    
    file.close();
    Logger::info(QString("签名库已保存: %1 条记录").arg(m_signatures.size()));
    return true;
}

int SignatureDB::getSignatureCount() const {
    QMutexLocker locker(&m_mutex);
    return m_signatures.size();
}

} // namespace WSS
