#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QMutex>

namespace WSS {

class SignatureDB : public QObject {
    Q_OBJECT

public:
    static SignatureDB* instance();
    
    bool loadDatabase(const QString& dbPath);
    bool isMalware(const QString& hash) const;
    QString getThreatName(const QString& hash) const;
    int getSeverity(const QString& hash) const;
    
    void addSignature(const QString& hash, const QString& name, int severity);
    bool saveDatabase(const QString& dbPath);
    
    int getSignatureCount() const;
    
private:
    explicit SignatureDB(QObject* parent = nullptr);
    ~SignatureDB();
    
    struct Signature {
        QString name;
        int severity;
    };
    
    QMap<QString, Signature> m_signatures;
    mutable QMutex m_mutex;
    static SignatureDB* s_instance;
};

} // namespace WSS
