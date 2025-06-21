// ChatTool/ client.cpp
// Client
// Copyright (c) 2025 yzymc
#include "common_socket.h"
#include "config_parser.h"
#include <iostream>
#include <thread>
#include <cstring>
#define _CRT_SECURE_NO_WARNINGS
#ifdef _WIN32
#include <windows.h>
#endif

std::string username;

void receive_messages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        std::cout << "\n" << buffer << "\n> " << std::flush;
    }
    std::cerr << "\nDisconnected from server.\n";
    exit(0);
}

int main()
{
#ifdef _WIN32
    // 设置控制台输入输出为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false); // 加速 & 修复控制台同步问题
#endif
    if (!init_socket_system()) {
        std::cerr << "Socket system init failed\n";
        return 1;
    }

    Config cfg;
    if (!cfg.load("client.ini")) {
        std::cerr << "Failed to load client.ini\n";
        return 1;
    }

    std::string host = cfg.get("network", "host", "127.0.0.1");
    int port = cfg.get_int("network", "port", 12345);
    username = cfg.get("user", "name", "");

    if (username.empty()) {
        std::cerr << "Username is required in client.ini\n";
        return 1;
    }

    addrinfo hints{}, * res = nullptr;
    hints.ai_family = AF_UNSPEC;       // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0) {
        std::cerr << "getaddrinfo() failed for " << host << "\n";
        return 1;
    }

    SOCKET sock = INVALID_SOCKET;
    for (addrinfo* ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) continue;
        if (connect(sock, ptr->ai_addr, ptr->ai_addrlen) == 0) break;
        close_socket(sock);
        sock = INVALID_SOCKET;
    }

    freeaddrinfo(res);

    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    // 发送用户名
    send(sock, username.c_str(), username.length(), 0);

    std::thread(receive_messages, sock).detach();

    std::string msg;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        send(sock, msg.c_str(), msg.length(), 0);
    }

    close_socket(sock);
    cleanup_socket_system();
    return 0;
}
