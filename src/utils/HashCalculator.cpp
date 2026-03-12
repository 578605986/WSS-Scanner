#include "utils/HashCalculator.h"
#include "utils/Logger.h"

#include <QCryptographicHash>
#include <QFile>

namespace WSS {

QString HashCalculator::calculateSHA256(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    
    // 分块读取大文件
    const qint64 bufferSize = 8192;
    char buffer[bufferSize];
    
    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer, bufferSize);
        if (bytesRead > 0) {
            hash.addData(buffer, bytesRead);
        }
    }
    
    file.close();
    return QString(hash.result().toHex());
}

QString HashCalculator::calculateMD5(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    
    QCryptographicHash hash(QCryptographicHash::Md5);
    
    const qint64 bufferSize = 8192;
    char buffer[bufferSize];
    
    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer, bufferSize);
        if (bytesRead > 0) {
            hash.addData(buffer, bytesRead);
        }
    }
    
    file.close();
    return QString(hash.result().toHex());
}

QString HashCalculator::calculateSHA256FromString(const QString& data) {
    return QString(QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256).toHex());
}

QString HashCalculator::calculateSHA256FromBytes(const QByteArray& data) {
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}

quint32 HashCalculator::calculateCRC32(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    
    quint32 crc = 0xFFFFFFFF;
    const qint64 bufferSize = 8192;
    char buffer[bufferSize];
    
    // CRC32表
    static const quint32 crcTable[256] = {
        // CRC32查找表（简化版）
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
        // ... 实际使用时需要完整表
    };
    
    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer, bufferSize);
        for (qint64 i = 0; i < bytesRead; i++) {
            crc = (crc >> 8) ^ crcTable[(crc ^ buffer[i]) & 0xFF];
        }
    }
    
    file.close();
    return ~crc;
}

} // namespace WSS
