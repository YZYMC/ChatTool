// ChatTool_Server/ server.cpp
// Server
// Copyright (c) 2025 yzymc
#include "common_socket.h"
#include "config_parser.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <map>

std::mutex clients_mutex;
std::map<SOCKET, std::string> client_names;

void handle_client(SOCKET client_sock) {
    char buffer[1024];

    // 第一步：接收用户名
    int name_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (name_len <= 0) {
        close_socket(client_sock);
        return;
    }
    buffer[name_len] = '\0';
    std::string username = buffer;
    if (username.empty()) {
        std::cerr << "Client with empty username rejected.\n";
        close_socket(client_sock);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_names[client_sock] = username;
    }

    std::cout << username << " joined the chat.\n";

    // 通知其他人
    std::string join_msg = "[Server] " + username + " has joined the chat.";
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& [sock, _] : client_names) {
            if (sock != client_sock) {
                send(sock, join_msg.c_str(), join_msg.length(), 0);
            }
        }
    }

    // 开始接收消息
    while (true) {
        int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';

        std::string msg_to_send = username + ": " + buffer;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& [sock, _] : client_names) {
            if (sock != client_sock) {
                send(sock, msg_to_send.c_str(), msg_to_send.length(), 0);
            }
        }
    }

    // 用户离开处理
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        std::string leave_msg = "[Server] " + username + " left the chat.";
        client_names.erase(client_sock);
        for (auto& [sock, _] : client_names) {
            send(sock, leave_msg.c_str(), leave_msg.length(), 0);
        }
    }

    std::cout << username << " disconnected.\n";
    close_socket(client_sock);
}

int main() {
    if (!init_socket_system()) {
        std::cerr << "Socket system init failed\n";
        return 1;
    }

    Config cfg;
    if (!cfg.load("server.ini")) {
        std::cerr << "Failed to load server.ini\n";
        return 1;
    }

    int port = cfg.get_int("network", "port", 12345);

    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0 ||
        listen(server_sock, 5) < 0) {
        std::cerr << "Bind or listen failed\n";
        close_socket(server_sock);
        cleanup_socket_system();
        return 1;
    }

    std::cout << "Server started on port " << port << "...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        SOCKET client_sock = accept(server_sock, (sockaddr*)&client_addr, &addr_len);
        if (client_sock == INVALID_SOCKET) continue;

        std::thread(handle_client, client_sock).detach();
    }

    close_socket(server_sock);
    cleanup_socket_system();
    return 0;
}
