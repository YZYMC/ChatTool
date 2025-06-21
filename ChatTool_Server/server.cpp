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

    // 获取用户名
    int name_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (name_len <= 0) {
        close_socket(client_sock);
        return;
    }
    buffer[name_len] = '\0';
    std::string username = buffer;
    if (username.empty()) {
        std::cerr << "Empty username rejected.\n";
        close_socket(client_sock);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_names[client_sock] = username;
    }

    std::string join_msg = "[Server] " + username + " has joined.";
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& [sock, _] : client_names) {
            if (sock != client_sock)
                send(sock, join_msg.c_str(), join_msg.length(), 0);
        }
    }

    // 消息循环
    while (true) {
        int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';

        std::string full_msg = username + ": " + buffer;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& [sock, _] : client_names) {
            if (sock != client_sock)
                send(sock, full_msg.c_str(), full_msg.length(), 0);
        }
    }

    // 离线清理
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        std::string leave_msg = "[Server] " + username + " left.";
        client_names.erase(client_sock);
        for (auto& [sock, _] : client_names)
            send(sock, leave_msg.c_str(), leave_msg.length(), 0);
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

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (sockaddr*)&addr, sizeof(addr)) < 0 ||
        listen(server_sock, 5) < 0) {
        std::cerr << "Bind or listen failed\n";
        close_socket(server_sock);
        cleanup_socket_system();
        return 1;
    }

    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        SOCKET client_sock = accept(server_sock, (sockaddr*)&client_addr, &len);
        if (client_sock == INVALID_SOCKET) continue;

        std::thread(handle_client, client_sock).detach();
    }

    close_socket(server_sock);
    cleanup_socket_system();
    return 0;
}
