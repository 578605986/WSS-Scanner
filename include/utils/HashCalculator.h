#pragma once

#include <QString>

namespace WSS {

class HashCalculator {
public:
    // 计算文件 SHA256 哈希
    static QString calculateSHA256(const QString& filePath);
    
    // 计算文件 MD5 哈希
    static QString calculateMD5(const QString& filePath);
    
    // 计算字符串 SHA256
    static QString calculateSHA256FromString(const QString& data);
    
    // 计算字节数组 SHA256
    static QString calculateSHA256FromBytes(const QByteArray& data);
    
    // 计算文件 CRC32
    static quint32 calculateCRC32(const QString& filePath);
};

} // namespace WSS
