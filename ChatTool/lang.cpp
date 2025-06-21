#include "lang.h"
#include <map>
#include <locale>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#include <winnls.h>  // 为了 GetUserDefaultLocaleName 和 LOCALE_NAME_MAX_LENGTH
#endif

static std::string current_lang = "en";

static const std::map<std::string, std::map<std::string, std::string>> translations = {
    {
        "en", {
            {"join", "{0} has joined the chat."},
            {"leave", "{0} has left the chat."},
            {"connected", "Connected to server."},
            {"enter_msg", "Enter message:"},
            {"disconnected", "Disconnected from server."},
            {"help", "Available commands:\n  /tell [username] [message] - Private message\n  /list - List users\n  /quit - Exit"},
            {"list", "Online users:"},
            {"error_prefix", "[System] "}
        }
    },
    {
        "zh", {
            {"join", "{0} 加入了聊天。"},
            {"leave", "{0} 退出了聊天。"},
            {"connected", "已连接到服务器。"},
            {"enter_msg", "输入消息："},
            {"disconnected", "与服务器断开连接。"},
            {"help", "可用指令：\n  /tell [用户名] [消息] - 私聊\n  /list - 显示在线用户\n  /quit - 退出"},
            {"list", "当前在线用户："},
            {"error_prefix", "[系统] "}
        }
    }
};

void set_language(const std::string& code) {
    if (translations.count(code))
        current_lang = code;
    else
        current_lang = "en";
}

std::string detect_lang() {
#ifdef _WIN32
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH)) {
        std::wstring ws(localeName);
        if (ws.find(L"zh") == 0) return "zh";
    }
    return "en";
#else
    const char* lang = getenv("LANG");
    if (lang && std::string(lang).find("zh") != std::string::npos)
        return "zh";
    return "en";
#endif
}

std::string tr(const std::string& key) {
    auto it = translations.find(current_lang);
    if (it != translations.end()) {
        const auto& dict = it->second;
        auto fit = dict.find(key);
        if (fit != dict.end())
            return fit->second;
    }
    return "[" + key + "]";
}

std::string format(const std::string& fmt, const std::string& arg) {
    std::string result = fmt;
    size_t pos = result.find("{0}");
    if (pos != std::string::npos)
        result.replace(pos, 3, arg);
    return result;
}
