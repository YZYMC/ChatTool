// config_parser.h
#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>

class Config {
public:
    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;

        std::string line, section;
        while (std::getline(file, line)) {
            trim(line);
            if (line.empty() || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = trim_copy(line.substr(0, eq));
            std::string value = trim_copy(line.substr(eq + 1));

            data[section + "." + key] = value;
        }

        return true;
    }

    std::string get(const std::string& section, const std::string& key, const std::string& def = "") const {
        auto it = data.find(section + "." + key);
        return it != data.end() ? it->second : def;
    }

    int get_int(const std::string& section, const std::string& key, int def = 0) const {
        try {
            return std::stoi(get(section, key));
        }
        catch (...) {
            return def;
        }
    }

private:
    std::unordered_map<std::string, std::string> data;

    static void trim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    static std::string trim_copy(const std::string& s) {
        std::string copy = s;
        trim(copy);
        return copy;
    }
};
