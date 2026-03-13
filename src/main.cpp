#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QFileDialog>
#include <QThread>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <atomic>

// ========== Logger ==========
class Logger {
public:
    static void initialize(const QString& logPath = "wss.log") {
        m_logPath = logPath;
        m_initialized = true;
        info("日志系统初始化完成");
    }
    static void info(const QString& message) {
        writeLog("INFO", message);
        qDebug() << "[INFO]" << message;
    }
    static void warning(const QString& message) {
        writeLog("WARNING", message);
        qWarning() << "[WARNING]" << message;
    }
    static void error(const QString& message) {
        writeLog("ERROR", message);
        qCritical() << "[ERROR]" << message;
    }
private:
    static void writeLog(const QString& level, const QString& message) {
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
    static QString m_logPath;
    static QMutex m_mutex;
    static bool m_initialized;
};
QString Logger::m_logPath = "wss.log";
QMutex Logger::m_mutex;
bool Logger::m_initialized = false;

// ========== HashCalculator ==========
class HashCalculator {
public:
    static QString calculateSHA256(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return QString();
        
        QCryptographicHash hash(QCryptographicHash::Sha256);
        const qint64 bufferSize = 8192;
        char buffer[bufferSize];
        
        while (!file.atEnd()) {
            qint64 bytesRead = file.read(buffer, bufferSize);
            if (bytesRead > 0) hash.addData(buffer, bytesRead);
        }
        file.close();
        return QString(hash.result().toHex());
    }
};

// ========== FileScanner ==========
class FileScanner : public QThread {
    Q_OBJECT
public:
    explicit FileScanner(QObject* parent = nullptr) : QThread(parent) {}
    
    void setTargetPath(const QString& path) { m_targetPath = path; }
    void stopScan() { m_stopFlag = true; }
    int getScannedCount() const { return m_scannedCount.load(); }
    
signals:
    void scanStarted();
    void scanProgress(int current, const QString& currentFile);
    void scanCompleted(int scanned);
    void fileScanned(const QString& path, const QString& hash);
    
protected:
    void run() override {
        if (m_targetPath.isEmpty()) return;
        
        emit scanStarted();
        Logger::info("扫描开始: " + m_targetPath);
        
        m_stopFlag = false;
        m_scannedCount = 0;
        
        scanDirectory(m_targetPath);
        
        emit scanCompleted(m_scannedCount.load());
        Logger::info(QString("扫描完成: %1 文件").arg(m_scannedCount.load()));
    }
    
private:
    void scanDirectory(const QString& path) {
        QDir dir(path);
        QFileInfoList entries = dir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        
        for (const QFileInfo& fi : entries) {
            if (m_stopFlag) break;
            
            if (fi.isDir()) {
                scanDirectory(fi.absoluteFilePath());
            } else if (fi.isFile()) {
                scanFile(fi.absoluteFilePath());
            }
        }
    }
    
    void scanFile(const QString& filePath) {
        m_scannedCount++;
        QString hash = HashCalculator::calculateSHA256(filePath);
        emit scanProgress(m_scannedCount.load(), QFileInfo(filePath).fileName());
        if (!hash.isEmpty()) {
            emit fileScanned(filePath, hash);
        }
    }
    
    QString m_targetPath;
    std::atomic<bool> m_stopFlag{false};
    std::atomic<int> m_scannedCount{0};
};

// ========== MainWindow ==========
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("WSS - Windows Security Scanner v1.0");
        setMinimumSize(900, 600);
        
        auto* central = new QWidget(this);
        auto* mainLayout = new QVBoxLayout(central);
        
        // 标题
        auto* titleLabel = new QLabel("WSS 安全扫描器 v1.0", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // 进度条
        m_progressBar = new QProgressBar(this);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        mainLayout->addWidget(m_progressBar);
        
        // 日志区域
        m_logEdit = new QTextEdit(this);
        m_logEdit->setReadOnly(true);
        m_logEdit->setMaximumHeight(200);
        mainLayout->addWidget(m_logEdit);
        
        // 按钮区域
        auto* btnLayout = new QHBoxLayout();
        
        auto* selectBtn = new QPushButton("选择目录", this);
        connect(selectBtn, &QPushButton::clicked, this, &MainWindow::onSelectDirectory);
        btnLayout->addWidget(selectBtn);
        
        auto* scanBtn = new QPushButton("开始扫描", this);
        connect(scanBtn, &QPushButton::clicked, this, &MainWindow::onStartScan);
        btnLayout->addWidget(scanBtn);
        
        auto* stopBtn = new QPushButton("停止扫描", this);
        connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStopScan);
        btnLayout->addWidget(stopBtn);
        
        mainLayout->addLayout(btnLayout);
        
        setCentralWidget(central);
        
        // 初始化扫描器
        m_scanner = new FileScanner(this);
        connect(m_scanner, &FileScanner::scanStarted, this, [this]() {
            m_logEdit->append("【扫描开始】");
        });
        connect(m_scanner, &FileScanner::scanProgress, this, [this](int current, const QString& file) {
            m_progressBar->setValue(current % 100);
            if (current % 10 == 0) m_logEdit->append(QString("扫描: %1").arg(file));
        });
        connect(m_scanner, &FileScanner::fileScanned, this, [this](const QString& path, const QString& hash) {
            m_logEdit->append(QString("✓ %1").arg(QFileInfo(path).fileName()));
        });
        connect(m_scanner, &FileScanner::scanCompleted, this, [this](int scanned) {
            m_progressBar->setValue(100);
            m_logEdit->append(QString("【扫描完成】共 %1 个文件").arg(scanned));
        });
        
        Logger::initialize();
        Logger::info("主窗口初始化完成");
    }
    
private slots:
    void onSelectDirectory() {
        QString path = QFileDialog::getExistingDirectory(this, "选择扫描目录");
        if (!path.isEmpty()) {
            m_scanPath = path;
            m_logEdit->append(QString("已选择: %1").arg(path));
        }
    }
    
    void onStartScan() {
        if (m_scanPath.isEmpty()) {
            m_logEdit->append("请先选择目录！");
            return;
        }
        m_scanner->setTargetPath(m_scanPath);
        m_scanner->start();
    }
    
    void onStopScan() {
        m_scanner->stopScan();
        m_logEdit->append("扫描已停止");
    }
    
private:
    QTextEdit* m_logEdit;
    QProgressBar* m_progressBar;
    FileScanner* m_scanner;
    QString m_scanPath;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
