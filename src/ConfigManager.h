#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <vector> // Added missing include for std::vector

/**
 * 配置管理器类
 * 负责应用程序配置的加载、保存和管理
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    /**
     * 加载配置文件
     * @param configPath 配置文件路径
     * @return 是否加载成功
     */
    bool loadConfig(const std::string& configPath);
    
    /**
     * 保存配置文件
     * @param configPath 配置文件路径
     * @return 是否保存成功
     */
    bool saveConfig(const std::string& configPath);
    
    /**
     * 获取字符串配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * 设置字符串配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setString(const std::string& key, const std::string& value);
    
    /**
     * 获取整数配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * 设置整数配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setInt(const std::string& key, int value);
    
    /**
     * 获取布尔配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * 设置布尔配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setBool(const std::string& key, bool value);
    
    /**
     * 获取浮点数配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * 设置浮点数配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setDouble(const std::string& key, double value);
    
    /**
     * 检查配置项是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * 删除配置项
     * @param key 配置键
     * @return 是否删除成功
     */
    bool removeKey(const std::string& key);
    
    /**
     * 获取所有配置键
     * @return 配置键列表
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * 清空所有配置
     */
    void clear();
    
    /**
     * 设置配置变化回调
     * @param key 配置键
     * @param callback 回调函数
     */
    void setConfigChangedCallback(const std::string& key, std::function<void(const std::string&)> callback);
    
    /**
     * 移除配置变化回调
     * @param key 配置键
     */
    void removeConfigChangedCallback(const std::string& key);
    
    /**
     * 重置为默认配置
     */
    void resetToDefaults();
    
    /**
     * 获取配置组
     * @param groupName 组名
     * @return 配置管理器指针
     */
    std::shared_ptr<ConfigManager> getGroup(const std::string& groupName);
    
    /**
     * 设置配置组
     * @param groupName 组名
     * @param config 配置管理器
     */
    void setGroup(const std::string& groupName, std::shared_ptr<ConfigManager> config);

private:
    std::unordered_map<std::string, std::string> stringValues_;
    std::unordered_map<std::string, int> intValues_;
    std::unordered_map<std::string, bool> boolValues_;
    std::unordered_map<std::string, double> doubleValues_;
    std::unordered_map<std::string, std::shared_ptr<ConfigManager>> groups_;
    std::unordered_map<std::string, std::function<void(const std::string&)>> callbacks_;
    
    /**
     * 通知配置变化
     * @param key 配置键
     */
    void notifyConfigChanged(const std::string& key);
    
    /**
     * 解析配置文件
     * @param content 文件内容
     * @return 是否解析成功
     */
    bool parseConfig(const std::string& content);
    
    /**
     * 生成配置文件内容
     * @return 配置文件内容
     */
    std::string generateConfig() const;
    
    /**
     * 转义字符串
     * @param str 原始字符串
     * @return 转义后的字符串
     */
    std::string escapeString(const std::string& str) const;
    
    /**
     * 反转义字符串
     * @param str 转义后的字符串
     * @return 原始字符串
     */
    std::string unescapeString(const std::string& str) const;
};

#endif // CONFIG_MANAGER_H
