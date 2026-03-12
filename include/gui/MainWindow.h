#pragma once

#include <QMainWindow>
#include <QThread>
#include <memory>

QT_BEGIN_NAMESPACE
class QPushButton;
class QProgressBar;
class QLabel;
class QTextEdit;
class QTableWidget;
class QTimer;
QT_END_NAMESPACE

namespace WSS {

class FileScanner;
class RealTimeMonitor;
class QuarantineManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onScanClicked();
    void onQuickScanClicked();
    void onFullScanClicked();
    void onMonitorClicked();
    void onQuarantineClicked();
    void onSettingsClicked();
    void onAboutClicked();
    
    void onScanStarted();
    void onScanProgress(int current, int total, const QString& file);
    void onThreatFound(const ThreatInfo& threat);
    void onScanCompleted(int scanned, int threats);
    
    void updateStatus();
    void onMonitorStatusChanged(bool running);

private:
    void setupUI();
    void setupConnections();
    void setupMenu();
    
    // UI 组件
    QPushButton* m_scanButton = nullptr;
    QPushButton* m_quickScanButton = nullptr;
    QPushButton* m_fullScanButton = nullptr;
    QPushButton* m_monitorButton = nullptr;
    QPushButton* m_quarantineButton = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_statsLabel = nullptr;
    QTextEdit* m_logEdit = nullptr;
    QTableWidget* m_threatTable = nullptr;
    
    // 功能模块
    std::unique_ptr<FileScanner> m_scanner;
    std::unique_ptr<RealTimeMonitor> m_monitor;
    std::unique_ptr<QuarantineManager> m_quarantine;
    
    QTimer* m_statusTimer = nullptr;
    bool m_isMonitoring = false;
};

} // namespace WSS
