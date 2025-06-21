// ChatTool/ client.cpp
// Client
// Copyright (c) 2025 yzymc
#include "common_socket.h"
#include "config_parser.h"
#include <iostream>
#include <thread>
#include <cstring>
#define _CRT_SECURE_NO_WARNINGS

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

int main() {
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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    hostent* server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << "\n";
        close_socket(sock);
        cleanup_socket_system();
        return 1;
    }

    std::memcpy(&server_addr.sin_addr, server->h_addr, server->h_length);

    std::cout << "Connecting to " << host << ":" << port << "...\n";
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        close_socket(sock);
        cleanup_socket_system();
        return 1;
    }

    // 发送用户名作为第一条消息
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
