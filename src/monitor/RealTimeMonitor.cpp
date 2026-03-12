#include "monitor/RealTimeMonitor.h"
#include "utils/Logger.h"

namespace WSS {

RealTimeMonitor::RealTimeMonitor(QObject* parent) : QObject(parent) {
}

RealTimeMonitor::~RealTimeMonitor() {
    stopMonitoring();
}

bool RealTimeMonitor::startMonitoring() {
    Logger::info("启动实时监控...");
    m_running = true;
    emit monitorStatusChanged(true);
    // TODO: 实现文件系统监控
    return true;
}

void RealTimeMonitor::stopMonitoring() {
    Logger::info("停止实时监控");
    m_running = false;
    emit monitorStatusChanged(false);
}

bool RealTimeMonitor::isRunning() const {
    return m_running;
}

} // namespace WSS
