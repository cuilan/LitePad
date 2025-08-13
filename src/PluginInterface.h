#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <string>

/**
 * 插件接口类
 * 所有插件都必须实现此接口
 */
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    /**
     * 获取插件名称
     * @return 插件名称
     */
    virtual std::string getName() const = 0;
    
    /**
     * 获取插件版本
     * @return 插件版本
     */
    virtual std::string getVersion() const = 0;
    
    /**
     * 获取插件描述
     * @return 插件描述
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * 初始化插件
     * @return 是否初始化成功
     */
    virtual bool initialize() = 0;
    
    /**
     * 清理插件资源
     */
    virtual void cleanup() = 0;
    
    /**
     * 启用插件
     */
    virtual void enable() = 0;
    
    /**
     * 禁用插件
     */
    virtual void disable() = 0;
    
    /**
     * 检查插件是否启用
     * @return 是否启用
     */
    virtual bool isEnabled() const = 0;
};

// 插件导出宏定义
#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

// 插件入口点函数类型
extern "C" {
    typedef PluginInterface* (*PluginCreateFunc)();
    typedef void (*PluginDestroyFunc)(PluginInterface*);
}

// 插件入口点函数声明
extern "C" {
    PLUGIN_EXPORT PluginInterface* createPlugin();
    PLUGIN_EXPORT void destroyPlugin(PluginInterface* plugin);
}

#endif // PLUGIN_INTERFACE_H
