#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("WSS - Windows Security Scanner");
    window.setMinimumSize(800, 600);
    
    auto* central = new QWidget(&window);
    auto* layout = new QVBoxLayout(central);
    
    auto* label = new QLabel("WSS 安全扫描器 v1.0");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    
    auto* btn = new QPushButton("开始扫描");
    layout->addWidget(btn);
    
    window.setCentralWidget(central);
    window.show();
    
    return app.exec();
}
