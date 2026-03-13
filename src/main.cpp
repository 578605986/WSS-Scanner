#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QDebug>

// Logger 工具类
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

// 主窗口
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("WSS - Windows Security Scanner v1.0");
        setMinimumSize(800, 600);
        
        auto* central = new QWidget(this);
        auto* layout = new QVBoxLayout(central);
        
        auto* label = new QLabel("WSS 安全扫描器 v1.0", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        
        m_logEdit = new QTextEdit(this);
        m_logEdit->setReadOnly(true);
        m_logEdit->setMaximumHeight(200);
        layout->addWidget(m_logEdit);
        
        auto* btn = new QPushButton("测试日志", this);
        connect(btn, &QPushButton::clicked, this, [this]() {
            Logger::info("测试按钮被点击");
            m_logEdit->append("日志已记录，请查看 wss.log 文件");
        });
        layout->addWidget(btn);
        
        setCentralWidget(central);
        
        Logger::initialize();
        Logger::info("主窗口初始化完成");
    }
    
private:
    QTextEdit* m_logEdit;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"
