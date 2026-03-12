#include "scanner/FileScanner.h"
#include "scanner/SignatureDB.h"
#include "utils/HashCalculator.h"
#include "utils/Logger.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

namespace WSS {

FileScanner::FileScanner(QObject* parent)
    : QThread(parent)
    , m_options()
{
}

FileScanner::~FileScanner() {
    stopScan();
    wait();
}

void FileScanner::setTargetPath(const QString& path) {
    m_targetPath = path;
}

void FileScanner::setOptions(const ScanOptions& options) {
    m_options = options;
}

void FileScanner::stopScan() {
    m_stopFlag = true;
}

QVector<ThreatInfo> FileScanner::getThreats() const {
    QMutexLocker locker(&m_mutex);
    return m_threats;
}

int FileScanner::getScannedCount() const {
    return m_scannedCount.load();
}

int FileScanner::getThreatCount() const {
    QMutexLocker locker(&m_mutex);
    return m_threats.size();
}

void FileScanner::run() {
    if (m_targetPath.isEmpty()) {
        emit scanError("目标路径为空");
        return;
    }
    
    emit scanStarted();
    Logger::info("扫描开始: " + m_targetPath);
    
    m_stopFlag = false;
    m_scannedCount = 0;
    
    QFileInfo fi(m_targetPath);
    if (fi.isDir()) {
        scanDirectory(m_targetPath);
    } else if (fi.isFile()) {
        scanFile(m_targetPath);
    }
    
    emit scanCompleted(m_scannedCount.load(), getThreatCount());
    Logger::info(QString("扫描完成: %1 文件, %2 威胁")
        .arg(m_scannedCount.load())
        .arg(getThreatCount()));
}

void FileScanner::scanDirectory(const QString& path) {
    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
        QDir::Name
    );
    
    for (const QFileInfo& fi : entries) {
        if (m_stopFlag) break;
        
        if (fi.isDir()) {
            // 递归扫描子目录
            scanDirectory(fi.absoluteFilePath());
        } else if (fi.isFile()) {
            scanFile(fi.absoluteFilePath());
        }
        
        m_scannedCount++;
        emit scanProgress(m_scannedCount.load(), 0, fi.fileName());
    }
}

void FileScanner::scanFile(const QString& filePath) {
    try {
        // 计算文件哈希
        QString hash = HashCalculator::calculateSHA256(filePath);
        if (hash.isEmpty()) return;
        
        // 查询签名库
        auto* db = SignatureDB::instance();
        if (db->isMalware(hash)) {
            ThreatInfo threat;
            threat.filePath = filePath;
            threat.threatName = db->getThreatName(hash);
            threat.threatType = "Signature";
            threat.severity = db->getSeverity(hash);
            threat.hash = hash;
            threat.quarantined = false;
            
            QMutexLocker locker(&m_mutex);
            m_threats.append(threat);
            
            emit threatFound(threat);
            Logger::warning(QString("发现威胁: %1 - %2")
                .arg(filePath)
                .arg(threat.threatName));
        }
        
        // TODO: 启发式分析
        if (m_options.heuristicAnalysis) {
            // heuristicScan(filePath);
        }
        
    } catch (...) {
        Logger::error("扫描文件异常: " + filePath);
    }
}

bool FileScanner::isArchive(const QString& filePath) {
    QString ext = QFileInfo(filePath).suffix().toLower();
    return ext == "zip" || ext == "rar" || ext == "7z" || 
           ext == "tar" || ext == "gz" || ext == "bz2";
}

void FileScanner::scanArchive(const QString& archivePath) {
    if (!m_options.scanArchives) return;
    
    // TODO: 实现压缩包扫描
    Logger::info("扫描压缩包: " + archivePath);
}

} // namespace WSS
