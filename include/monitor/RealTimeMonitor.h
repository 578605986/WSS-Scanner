#pragma once

#include <QObject>
#include <QThread>

namespace WSS {

class RealTimeMonitor : public QObject {
    Q_OBJECT

public:
    explicit RealTimeMonitor(QObject* parent = nullptr);
    ~RealTimeMonitor();
    
    bool startMonitoring();
    void stopMonitoring();
    bool isRunning() const;

signals:
    void fileModified(const QString& path);
    void threatDetected(const QString& path, const QString& reason);
    void monitorStatusChanged(bool running);

private:
    bool m_running = false;
};

} // namespace WSS
