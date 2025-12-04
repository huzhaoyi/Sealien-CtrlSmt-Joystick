#!/usr/bin/env python3
"""
Sealien 手柄转换器 - 简单的HTTP服务器
用于提供Web控制面板的静态文件服务
"""

import http.server
import socketserver
import sys
import os
from urllib.parse import urlparse

class FilteredHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    """过滤掉Chrome DevTools等无用的404请求"""
    
    # 需要忽略的路径模式
    IGNORE_PATTERNS = [
        '/.well-known/',
        '/favicon.ico',
    ]
    
    def log_message(self, format, *args):
        """自定义日志输出，过滤掉无用的404请求"""
        # log_message的参数格式通常是: (client_address, date_time, request_line, status_code, size)
        # 例如: ('127.0.0.1', '14/Nov/2025 15:08:56', 'GET /path HTTP/1.1', '404', '-')
        if len(args) >= 3:
            request_line = args[2]  # 请求行，例如 "GET /path HTTP/1.1"
            status_code = args[3] if len(args) > 3 else None
            
            # 检查是否是404错误且路径应该被忽略
            if status_code == '404' and request_line:
                # 从请求行中提取路径
                parts = request_line.split()
                if len(parts) >= 2:
                    path = parts[1]  # 提取路径部分
                    if any(pattern in path for pattern in self.IGNORE_PATTERNS):
                        # 忽略这些请求的日志
                        return
        
        # 输出其他请求的日志
        super().log_message(format, *args)
    
    def end_headers(self):
        """添加CORS头，允许跨域请求"""
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()

def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法: python3 http_server.py <端口> [目录]")
        sys.exit(1)
    
    try:
        port = int(sys.argv[1])
    except ValueError:
        print(f"错误: 无效的端口号: {sys.argv[1]}")
        sys.exit(1)
    
    # 设置工作目录
    if len(sys.argv) >= 3:
        os.chdir(sys.argv[2])
    
    # 创建服务器
    with socketserver.TCPServer(("", port), FilteredHTTPRequestHandler) as httpd:
        print(f"Serving HTTP on 0.0.0.0 port {port} (http://0.0.0.0:{port}/) ...")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nHTTP服务器已停止")
            httpd.shutdown()

if __name__ == '__main__':
    main()

