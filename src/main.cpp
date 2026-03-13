#include <QApplication>
#include <QMessageBox>
#include "gui/MainWindow.h"
#include "scanner/SignatureDB.h"
#include "utils/Logger.h"

int main(int argc, char *argv[]) {
    // 启用高DPI支持
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
    );
    
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("WSS - Windows Security Scanner");
    app.setOrganizationName("WSS-Security");
    app.setApplicationVersion("1.0.0");
    
    // 初始化日志
    WSS::Logger::initialize();
    WSS::Logger::info("WSS 启动");
    
    // 加载病毒签名库
    auto* sigDB = WSS::SignatureDB::instance();
    if (!sigDB->loadDatabase("signatures/wss.db")) {
        QMessageBox::warning(nullptr, "警告", 
            "病毒签名库加载失败，将使用空数据库。\\n"
            "请确保 signatures/wss.db 文件存在。");
    }
    
    // 创建主窗口
    WSS::MainWindow window;
    window.show();
    
    return app.exec();
}
