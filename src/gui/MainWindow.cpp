#include "gui/MainWindow.h"
#include "scanner/FileScanner.h"
#include "utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QStatusBar>

namespace WSS {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_scanner(std::make_unique<FileScanner>())
    , m_monitor(std::make_unique<RealTimeMonitor>())
    , m_quarantine(std::make_unique<QuarantineManager>())
{
    setupUI();
    setupConnections();
    setupMenu();
    
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    m_statusTimer->start(1000);
    
    Logger::info("主窗口初始化完成");
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("WSS - Windows Security Scanner v1.0");
    setMinimumSize(900, 600);
    
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* mainLayout = new QVBoxLayout(centralWidget);
    
    // 按钮区域
    auto* buttonLayout = new QHBoxLayout();
    
    m_quickScanButton = new QPushButton("快速扫描", this);
    m_quickScanButton->setToolTip("扫描常用目录");
    m_quickScanButton->setMinimumHeight(40);
    
    m_fullScanButton = new QPushButton("全盘扫描", this);
    m_fullScanButton->setToolTip("扫描所有磁盘");
    m_fullScanButton->setMinimumHeight(40);
    
    m_scanButton = new QPushButton("自定义扫描", this);
    m_scanButton->setToolTip("选择目录扫描");
    m_scanButton->setMinimumHeight(40);
    
    m_monitorButton = new QPushButton("启动监控", this);
    m_monitorButton->setCheckable(true);
    m_monitorButton->setMinimumHeight(40);
    
    m_quarantineButton = new QPushButton("隔离区", this);
    m_quarantineButton->setMinimumHeight(40);
    
    buttonLayout->addWidget(m_quickScanButton);
    buttonLayout->addWidget(m_fullScanButton);
    buttonLayout->addWidget(m_scanButton);
    buttonLayout->addWidget(m_monitorButton);
    buttonLayout->addWidget(m_quarantineButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);
    
    // 威胁列表
    m_threatTable = new QTableWidget(this);
    m_threatTable->setColumnCount(4);
    m_threatTable->setHorizontalHeaderLabels(
        QStringList() <> "文件路径" <> "威胁名称" <> "风险等级" <> "操作");
    m_threatTable->horizontalHeader()->setStretchLastSection(true);
    m_threatTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(m_threatTable);
    
    // 日志区域
    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(150);
    mainLayout->addWidget(m_logEdit);
    
    // 状态栏
    m_statusLabel = new QLabel("就绪", this);
    m_statsLabel = new QLabel("已扫描: 0 | 威胁: 0", this);
    
    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_statsLabel);
}

void MainWindow::setupConnections() {
    connect(m_scanButton, &QPushButton::clicked, this, &MainWindow::onScanClicked);
    connect(m_quickScanButton, &QPushButton::clicked, this, &MainWindow::onQuickScanClicked);
    connect(m_fullScanButton, &QPushButton::clicked, this, &MainWindow::onFullScanClicked);
    connect(m_monitorButton, &QPushButton::toggled, this, &MainWindow::onMonitorClicked);
    connect(m_quarantineButton, &QPushButton::clicked, this, &MainWindow::onQuarantineClicked);
    
    // 扫描器信号
    connect(m_scanner.get(), &FileScanner::scanStarted, this, &MainWindow::onScanStarted);
    connect(m_scanner.get(), &FileScanner::scanProgress, this, &MainWindow::onScanProgress);
    connect(m_scanner.get(), &FileScanner::threatFound, this, &MainWindow::onThreatFound);
    connect(m_scanner.get(), &FileScanner::scanCompleted, this, &MainWindow::onScanCompleted);
}

void MainWindow::setupMenu() {
    auto* fileMenu = menuBar()->addMenu("文件");
    auto* settingsAction = fileMenu->addAction("设置");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    fileMenu->addSeparator();
    auto* exitAction = fileMenu->addAction("退出");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    auto* helpMenu = menuBar()->addMenu("帮助");
    auto* aboutAction = helpMenu->addAction("关于");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

void MainWindow::onScanClicked() {
    QString path = QFileDialog::getExistingDirectory(this, "选择扫描目录");
    if (path.isEmpty()) return;
    
    m_scanner->setTargetPath(path);
    ScanOptions options;
    options.threadCount = 4;
    m_scanner->setOptions(options);
    
    m_threatTable->setRowCount(0);
    m_scanner->start();
}

void MainWindow::onQuickScanClicked() {
    // 扫描常用目录
    m_scanner->setTargetPath("C:/Users");
    ScanOptions options;
    options.threadCount = 4;
    m_scanner->setOptions(options);
    
    m_threatTable->setRowCount(0);
    m_scanner->start();
}

void MainWindow::onFullScanClicked() {
    auto ret = QMessageBox::question(this, "确认", 
        "全盘扫描可能需要较长时间，是否继续？");
    if (ret != QMessageBox::Yes) return;
    
    m_scanner->setTargetPath("C:/");
    ScanOptions options;
    options.threadCount = 4;
    m_scanner->setOptions(options);
    
    m_threatTable->setRowCount(0);
    m_scanner->start();
}

void MainWindow::onMonitorClicked(bool checked) {
    m_isMonitoring = checked;
    m_monitorButton->setText(checked ? "停止监控" : "启动监控");
    onMonitorStatusChanged(checked);
    
    if (checked) {
        Logger::info("实时监控已启动");
        m_statusLabel->setText("监控中...");
    } else {
        Logger::info("实时监控已停止");
        m_statusLabel->setText("就绪");
    }
}

void MainWindow::onQuarantineClicked() {
    QMessageBox::information(this, "隔离区", "隔离区管理功能开发中...");
}

void MainWindow::onSettingsClicked() {
    QMessageBox::information(this, "设置", "设置功能开发中...");
}

void MainWindow::onAboutClicked() {
    QMessageBox::about(this, "关于",
        "WSS - Windows Security Scanner v1.0\n\n"
        "专业级Windows安全扫描工具\n"
        "支持文件扫描、实时监控、自动隔离\n\n"
        "© 2024 WSS Security");
}

void MainWindow::onScanStarted() {
    m_progressBar->setValue(0);
    m_statusLabel->setText("扫描中...");
    Logger::info("扫描开始");
}

void MainWindow::onScanProgress(int current, int total, const QString& file) {
    if (total > 0) {
        m_progressBar->setValue(current * 100 / total);
    } else {
        m_progressBar->setValue((current % 100));
    }
    m_statusLabel->setText("正在扫描: " + QFileInfo(file).fileName());
}

void MainWindow::onThreatFound(const ThreatInfo& threat) {
    int row = m_threatTable->rowCount();
    m_threatTable->insertRow(row);
    
    m_threatTable->setItem(row, 0, new QTableWidgetItem(threat.filePath));
    m_threatTable->setItem(row, 1, new QTableWidgetItem(threat.threatName));
    m_threatTable->setItem(row, 2, new QTableWidgetItem(QString::number(threat.severity)));
    
    auto* actionButton = new QPushButton("隔离", this);
    m_threatTable->setCellWidget(row, 3, actionButton);
    
    m_logEdit->append(QString("[威胁] %1 - %2").arg(threat.filePath, threat.threatName));
}

void MainWindow::onScanCompleted(int scanned, int threats) {
    m_progressBar->setValue(100);
    m_statusLabel->setText("扫描完成");
    m_statsLabel->setText(QString("已扫描: %1 | 威胁: %2").arg(scanned).arg(threats));
    
    Logger::info(QString("扫描完成: %1 文件, %2 威胁").arg(scanned).arg(threats));
    
    if (threats > 0) {
        QMessageBox::warning(this, "扫描完成", 
            QString("发现 %1 个威胁！请查看列表并处理。").arg(threats));
    } else {
        QMessageBox::information(this, "扫描完成", 
            "未发现威胁，系统安全。");
    }
}

void MainWindow::updateStatus() {
    // 更新状态信息
}

void MainWindow::onMonitorStatusChanged(bool running) {
    // 处理监控状态变化
}

} // namespace WSS
