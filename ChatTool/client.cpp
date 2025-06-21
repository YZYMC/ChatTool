// ChatTool/ client.cpp
// Client
// Copyright (c) 2025 yzymc
#include "common_socket.h"
#include "config_parser.h"
#include "lang.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
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
        std::string msg(buffer);

        if (msg.rfind("SYS:JOIN:", 0) == 0) {
            std::cout << format(tr("join"), msg.substr(9)) << "\n> " << std::flush;
        }
        else if (msg.rfind("SYS:LEAVE:", 0) == 0) {
            std::cout << format(tr("leave"), msg.substr(10)) << "\n> " << std::flush;
        }
        else if (msg.rfind("SYS:ERROR:", 0) == 0) {
            std::cout << tr("error_prefix") << msg.substr(10) << "\n> " << std::flush;
        }
        else if (msg.rfind("SYS:LIST:", 0) == 0) {
            std::cout << tr("list") << "\n" << msg.substr(9) << "\n> " << std::flush;
        }
        else {
            std::cout << "\n" << msg << "\n> " << std::flush;
        }
    }
    std::cerr << "\n" << tr("disconnected") << "\n";
    exit(0);
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // 设置控制台输入输出为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);
#endif

    std::string lang_code;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-lang" && i + 1 < argc) {
            lang_code = argv[++i];
        }
    }
    if (lang_code.empty()) lang_code = detect_lang();
    set_language(lang_code);

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
    hints.ai_family = AF_UNSPEC;
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

    std::cout << tr("connected") << "\n";

    // 发送用户名
    send(sock, username.c_str(), username.length(), 0);

    std::thread(receive_messages, sock).detach();

    std::string msg;
    while (true) {
        std::cout << tr("enter_msg") << " ";
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        else if (msg == "/help") {
            std::cout << tr("help") << "\n";
        }
        else {
            send(sock, msg.c_str(), msg.length(), 0);
        }
    }

    close_socket(sock);
    cleanup_socket_system();
    return 0;
}
