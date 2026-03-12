#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QThread>
#include <QMutex>
#include <functional>

namespace WSS {

struct ThreatInfo {
    QString filePath;
    QString threatName;
    QString threatType;
    int severity; // 1-5
    QString hash;
    bool quarantined;
};

struct ScanOptions {
    bool scanArchives = true;
    bool heuristicAnalysis = true;
    bool scanMemory = false;
    int threadCount = 4;
    QStringList excludePaths;
};

class FileScanner : public QThread {
    Q_OBJECT

public:
    explicit FileScanner(QObject* parent = nullptr);
    ~FileScanner();

    void setTargetPath(const QString& path);
    void setOptions(const ScanOptions& options);
    void stopScan();
    
    QVector<ThreatInfo> getThreats() const;
    int getScannedCount() const;
    int getThreatCount() const;

signals:
    void scanStarted();
    void scanProgress(int current, int total, const QString& currentFile);
    void threatFound(const ThreatInfo& threat);
    void scanCompleted(int scanned, int threats);
    void scanError(const QString& error);

protected:
    void run() override;

private:
    void scanDirectory(const QString& path);
    void scanFile(const QString& filePath);
    bool isArchive(const QString& filePath);
    void scanArchive(const QString& archivePath);
    
    QString m_targetPath;
    ScanOptions m_options;
    QVector<ThreatInfo> m_threats;
    mutable QMutex m_mutex;
    std::atomic<bool> m_stopFlag{false};
    std::atomic<int> m_scannedCount{0};
};

} // namespace WSS
