#pragma once
#include <string>

void set_language(const std::string& code); // "zh" 或 "en"
std::string detect_lang();                  // 自动检测系统语言
std::string tr(const std::string& key);     // 翻译某个键
std::string format(const std::string& fmt, const std::string& arg); // 替换{0}为arg
