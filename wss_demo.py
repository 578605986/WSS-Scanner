#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WSS - Windows Security Scanner (Python演示版)
快速演示版，无需编译即可运行
"""

import os
import sys
import hashlib
import json
from datetime import datetime
from pathlib import Path

try:
    from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                                  QHBoxLayout, QPushButton, QProgressBar, QLabel,
                                  QTextEdit, QTableWidget, QTableWidgetItem, QFileDialog,
                                  QMessageBox, QHeaderView)
    from PyQt6.QtCore import Qt, QThread, pyqtSignal
    GUI_AVAILABLE = True
except ImportError:
    GUI_AVAILABLE = False
    print("PyQt6未安装，将使用命令行版本")
    print("安装命令: pip install PyQt6")

# 模拟病毒签名库
SAMPLE_SIGNATURES = {
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855": 
        ("Test.Malware.A", 3),
    "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8": 
        ("Test.Malware.B", 4),
}

class ScannerThread(QThread):
    """扫描线程"""
    scan_started = pyqtSignal()
    scan_progress = pyqtSignal(int, int, str)
    threat_found = pyqtSignal(dict)
    scan_completed = pyqtSignal(int, int)
    
    def __init__(self, target_path):
        super().__init__()
        self.target_path = target_path
        self.running = True
        
    def calculate_sha256(self, filepath):
        """计算文件SHA256"""
        sha256_hash = hashlib.sha256()
        try:
            with open(filepath, "rb") as f:
                for byte_block in iter(lambda: f.read(4096), b""):
                    sha256_hash.update(byte_block)
            return sha256_hash.hexdigest()
        except:
            return None
    
    def run(self):
        self.scan_started.emit()
        
        scanned = 0
        threats = 0
        
        for root, dirs, files in os.walk(self.target_path):
            if not self.running:
                break
                
            for file in files:
                if not self.running:
                    break
                    
                filepath = os.path.join(root, file)
                scanned += 1
                
                # 计算哈希
                file_hash = self.calculate_sha256(filepath)
                if file_hash and file_hash in SAMPLE_SIGNATURES:
                    threat_name, severity = SAMPLE_SIGNATURES[file_hash]
                    threat_info = {
                        'file_path': filepath,
                        'threat_name': threat_name,
                        'severity': severity,
                        'hash': file_hash
                    }
                    self.threat_found.emit(threat_info)
                    threats += 1
                
                self.scan_progress.emit(scanned, 0, file)
        
        self.scan_completed.emit(scanned, threats)

class WSSMainWindow(QMainWindow):
    """主窗口"""
    def __init__(self):
        super().__init__()
        self.setWindowTitle("WSS - Windows Security Scanner (Python Demo)")
        self.setMinimumSize(800, 600)
        self.scanner_thread = None
        self.setup_ui()
        
    def setup_ui(self):
        # 中央部件
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        
        # 标题
        title = QLabel("Windows Security Scanner")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        title.setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;")
        layout.addWidget(title)
        
        # 按钮区域
        btn_layout = QHBoxLayout()
        
        self.btn_scan = QPushButton("选择目录扫描")
        self.btn_scan.setMinimumHeight(40)
        self.btn_scan.clicked.connect(self.start_scan)
        
        self.btn_quick = QPushButton("快速扫描")
        self.btn_quick.setMinimumHeight(40)
        self.btn_quick.clicked.connect(self.quick_scan)
        
        self.btn_about = QPushButton("关于")
        self.btn_about.setMinimumHeight(40)
        self.btn_about.clicked.connect(self.show_about)
        
        btn_layout.addWidget(self.btn_scan)
        btn_layout.addWidget(self.btn_quick)
        btn_layout.addWidget(self.btn_about)
        layout.addLayout(btn_layout)
        
        # 进度条
        self.progress = QProgressBar()
        self.progress.setRange(0, 100)
        layout.addWidget(self.progress)
        
        # 状态标签
        self.status_label = QLabel("就绪 - 点击按钮开始扫描")
        self.status_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.status_label)
        
        # 威胁列表
        self.threat_table = QTableWidget()
        self.threat_table.setColumnCount(4)
        self.threat_table.setHorizontalHeaderLabels(
            ["文件路径", "威胁名称", "风险等级", "哈希值"])
        self.threat_table.horizontalHeader().setStretchLastSection(True)
        layout.addWidget(self.threat_table)
        
        # 日志区域
        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setMaximumHeight(150)
        self.log.setText("WSS Scanner v1.0 (Python Demo)\n")
        self.log.append("功能：文件扫描、哈希比对、威胁检测\n")
        self.log.append("说明：此为演示版本，仅用于学习和测试\n")
        layout.addWidget(self.log)
        
    def start_scan(self):
        path = QFileDialog.getExistingDirectory(self, "选择扫描目录")
        if path:
            self.run_scanner(path)
    
    def quick_scan(self):
        # 扫描当前目录
        self.run_scanner(os.getcwd())
    
    def run_scanner(self, path):
        self.threat_table.setRowCount(0)
        self.status_label.setText(f"正在扫描: {path}")
        self.log.append(f"\n[{datetime.now().strftime('%H:%M:%S')}] 开始扫描: {path}")
        
        self.scanner_thread = ScannerThread(path)
        self.scanner_thread.scan_started.connect(self.on_scan_started)
        self.scanner_thread.scan_progress.connect(self.on_scan_progress)
        self.scanner_thread.threat_found.connect(self.on_threat_found)
        self.scanner_thread.scan_completed.connect(self.on_scan_completed)
        self.scanner_thread.start()
    
    def on_scan_started(self):
        self.progress.setValue(0)
        self.btn_scan.setEnabled(False)
        self.btn_quick.setEnabled(False)
    
    def on_scan_progress(self, current, total, filename):
        self.progress.setValue(current % 100)
        self.status_label.setText(f"正在扫描: {os.path.basename(filename)}")
    
    def on_threat_found(self, threat):
        row = self.threat_table.rowCount()
        self.threat_table.insertRow(row)
        self.threat_table.setItem(row, 0, QTableWidgetItem(threat['file_path']))
        self.threat_table.setItem(row, 1, QTableWidgetItem(threat['threat_name']))
        self.threat_table.setItem(row, 2, QTableWidgetItem(str(threat['severity'])))
        self.threat_table.setItem(row, 3, QTableWidgetItem(threat['hash'][:16] + "..."))
        
        self.log.append(f"[!] 发现威胁: {threat['file_path']} - {threat['threat_name']}")
    
    def on_scan_completed(self, scanned, threats):
        self.progress.setValue(100)
        self.status_label.setText(f"扫描完成: {scanned} 文件, {threats} 威胁")
        self.btn_scan.setEnabled(True)
        self.btn_quick.setEnabled(True)
        
        self.log.append(f"[{datetime.now().strftime('%H:%M:%S')}] 扫描完成")
        self.log.append(f"统计: {scanned} 文件已扫描, {threats} 威胁发现\n")
        
        if threats > 0:
            QMessageBox.warning(self, "扫描完成", 
                f"发现 {threats} 个威胁！\n请查看列表并处理。")
        else:
            QMessageBox.information(self, "扫描完成", 
                f"扫描完成！\n未发现威胁，系统安全。")
    
    def show_about(self):
        QMessageBox.about(self, "关于",
            "WSS - Windows Security Scanner\n"
            "Python演示版 v1.0\n\n"
            "功能：文件扫描、哈希比对、威胁检测\n\n"
            "说明：\n"
            "- 此为演示版本，用于学习C++项目架构\n"
            "- 实际使用请编译C++完整版\n"
            "- 可自由用于商业安全服务\n\n"
            "© 2024 WSS Security")

def main():
    if not GUI_AVAILABLE:
        # 命令行版本
        print("=" * 60)
        print("WSS - Windows Security Scanner (Python CLI Demo)")
        print("=" * 60)
        print()
        print("这是一个演示版本，完整功能请使用C++版本")
        print()
        print("如需GUI版本，请安装PyQt6:")
        print("  pip install PyQt6")
        print()
        return
    
    app = QApplication(sys.argv)
    window = WSSMainWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
