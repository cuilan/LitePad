#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

ConfigManager::ConfigManager() = default;

ConfigManager::~ConfigManager() = default;

bool ConfigManager::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return parseConfig(content);
}

bool ConfigManager::saveConfig(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content = generateConfig();
    file << content;
    file.close();
    
    return true;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = stringValues_.find(key);
    return it != stringValues_.end() ? it->second : defaultValue;
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    stringValues_[key] = value;
    notifyConfigChanged(key);
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    auto it = intValues_.find(key);
    return it != intValues_.end() ? it->second : defaultValue;
}

void ConfigManager::setInt(const std::string& key, int value) {
    intValues_[key] = value;
    notifyConfigChanged(key);
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    auto it = boolValues_.find(key);
    return it != boolValues_.end() ? it->second : defaultValue;
}

void ConfigManager::setBool(const std::string& key, bool value) {
    boolValues_[key] = value;
    notifyConfigChanged(key);
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    auto it = doubleValues_.find(key);
    return it != doubleValues_.end() ? it->second : defaultValue;
}

void ConfigManager::setDouble(const std::string& key, double value) {
    doubleValues_[key] = value;
    notifyConfigChanged(key);
}

bool ConfigManager::hasKey(const std::string& key) const {
    return stringValues_.find(key) != stringValues_.end() ||
           intValues_.find(key) != intValues_.end() ||
           boolValues_.find(key) != boolValues_.end() ||
           doubleValues_.find(key) != doubleValues_.end();
}

bool ConfigManager::removeKey(const std::string& key) {
    bool removed = false;
    
    auto stringIt = stringValues_.find(key);
    if (stringIt != stringValues_.end()) {
        stringValues_.erase(stringIt);
        removed = true;
    }
    
    auto intIt = intValues_.find(key);
    if (intIt != intValues_.end()) {
        intValues_.erase(intIt);
        removed = true;
    }
    
    auto boolIt = boolValues_.find(key);
    if (boolIt != boolValues_.end()) {
        boolValues_.erase(boolIt);
        removed = true;
    }
    
    auto doubleIt = doubleValues_.find(key);
    if (doubleIt != doubleValues_.end()) {
        doubleValues_.erase(doubleIt);
        removed = true;
    }
    
    if (removed) {
        notifyConfigChanged(key);
    }
    
    return removed;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::vector<std::string> keys;
    
    for (const auto& pair : stringValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : intValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : boolValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : doubleValues_) {
        keys.push_back(pair.first);
    }
    
    // 去重
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    
    return keys;
}

void ConfigManager::clear() {
    stringValues_.clear();
    intValues_.clear();
    boolValues_.clear();
    doubleValues_.clear();
    groups_.clear();
    callbacks_.clear();
}

void ConfigManager::setConfigChangedCallback(const std::string& key, std::function<void(const std::string&)> callback) {
    callbacks_[key] = callback;
}

void ConfigManager::removeConfigChangedCallback(const std::string& key) {
    callbacks_.erase(key);
}

void ConfigManager::resetToDefaults() {
    clear();
    // TODO: 加载默认配置
}

std::shared_ptr<ConfigManager> ConfigManager::getGroup(const std::string& groupName) {
    auto it = groups_.find(groupName);
    if (it != groups_.end()) {
        return it->second;
    }
    
    auto group = std::make_shared<ConfigManager>();
    groups_[groupName] = group;
    return group;
}

void ConfigManager::setGroup(const std::string& groupName, std::shared_ptr<ConfigManager> config) {
    groups_[groupName] = config;
}

void ConfigManager::notifyConfigChanged(const std::string& key) {
    auto it = callbacks_.find(key);
    if (it != callbacks_.end()) {
        it->second(key);
    }
}

bool ConfigManager::parseConfig(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        // 移除前后空白字符
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // 忽略空行和注释
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // 处理节
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // 处理键值对
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // 移除键值的空白字符
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // 添加节前缀
        if (!currentSection.empty()) {
            key = currentSection + "." + key;
        }
        
        // 尝试解析不同类型的值
        if (value == "true" || value == "false") {
            setBool(key, value == "true");
        } else if (value.find('.') != std::string::npos) {
            try {
                double doubleValue = std::stod(value);
                setDouble(key, doubleValue);
            } catch (...) {
                setString(key, unescapeString(value));
            }
        } else {
            try {
                int intValue = std::stoi(value);
                setInt(key, intValue);
            } catch (...) {
                setString(key, unescapeString(value));
            }
        }
    }
    
    return true;
}

std::string ConfigManager::generateConfig() const {
    std::ostringstream stream;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> sections;
    
    // 收集所有键值对并按节分组
    for (const auto& pair : stringValues_) {
        std::string section, key;
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            section = pair.first.substr(0, dotPos);
            key = pair.first.substr(dotPos + 1);
        } else {
            section = "";
            key = pair.first;
        }
        sections[section].emplace_back(key, escapeString(pair.second));
    }
    
    for (const auto& pair : intValues_) {
        std::string section, key;
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            section = pair.first.substr(0, dotPos);
            key = pair.first.substr(dotPos + 1);
        } else {
            section = "";
            key = pair.first;
        }
        sections[section].emplace_back(key, std::to_string(pair.second));
    }
    
    for (const auto& pair : boolValues_) {
        std::string section, key;
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            section = pair.first.substr(0, dotPos);
            key = pair.first.substr(dotPos + 1);
        } else {
            section = "";
            key = pair.first;
        }
        sections[section].emplace_back(key, pair.second ? "true" : "false");
    }
    
    for (const auto& pair : doubleValues_) {
        std::string section, key;
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            section = pair.first.substr(0, dotPos);
            key = pair.first.substr(dotPos + 1);
        } else {
            section = "";
            key = pair.first;
        }
        sections[section].emplace_back(key, std::to_string(pair.second));
    }
    
    // 输出配置
    for (const auto& section : sections) {
        if (!section.first.empty()) {
            stream << "[" << section.first << "]\n";
        }
        
        for (const auto& keyValue : section.second) {
            stream << keyValue.first << " = " << keyValue.second << "\n";
        }
        
        stream << "\n";
    }
    
    return stream.str();
}

std::string ConfigManager::escapeString(const std::string& str) const {
    // 简单的转义实现
    std::string escaped = str;
    // TODO: 实现更完整的转义
    return escaped;
}

std::string ConfigManager::unescapeString(const std::string& str) const {
    // 简单的反转义实现
    std::string unescaped = str;
    // TODO: 实现更完整的反转义
    return unescaped;
}
