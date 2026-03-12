#pragma once

#include <QObject>
#include <QString>
#include <QVector>

namespace WSS {

struct QuarantinedItem {
    QString originalPath;
    QString quarantinePath;
    QString threatName;
    QString quarantineTime;
    bool restored = false;
};

class QuarantineManager : public QObject {
    Q_OBJECT

public:
    explicit QuarantineManager(QObject* parent = nullptr);
    
    bool quarantineFile(const QString& filePath, const QString& threatName);
    bool restoreFile(const QString& quarantineId);
    bool deleteFile(const QString& quarantineId);
    
    QVector<QuarantinedItem> getQuarantinedItems() const;
    
private:
    QString m_quarantineDir;
};

} // namespace WSS
