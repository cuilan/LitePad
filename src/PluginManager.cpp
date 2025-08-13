#include "PluginManager.h"
#include <iostream>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() {
    // 卸载所有插件
    for (auto& pair : plugins_) {
        if (pair.second.plugin) {
            pair.second.plugin->cleanup();
        }
        unloadLibrary(pair.second.libraryHandle);
    }
    plugins_.clear();
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    // 检查插件是否已经加载
    std::string pluginName = std::filesystem::path(pluginPath).stem().string();
    if (isPluginLoaded(pluginName)) {
        return false;
    }
    
    // 加载动态库
    void* handle = loadLibrary(pluginPath);
    if (!handle) {
        std::cerr << "Failed to load plugin library: " << pluginPath << std::endl;
        return false;
    }
    
    // 获取插件创建函数
    PluginCreateFunc createFunc = getCreateFunction(handle);
    if (!createFunc) {
        std::cerr << "Failed to find plugin create function in: " << pluginPath << std::endl;
        unloadLibrary(handle);
        return false;
    }
    
    // 创建插件实例
    PluginInterface* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance from: " << pluginPath << std::endl;
        unloadLibrary(handle);
        return false;
    }
    
    // 初始化插件
    if (!plugin->initialize()) {
        std::cerr << "Failed to initialize plugin: " << plugin->getName() << std::endl;
        delete plugin;
        unloadLibrary(handle);
        return false;
    }
    
    // 存储插件信息
    PluginInfo info;
    info.name = plugin->getName();
    info.path = pluginPath;
    info.plugin = std::shared_ptr<PluginInterface>(plugin);
    info.enabled = false;
    info.libraryHandle = handle;
    
    plugins_[info.name] = info;
    
    std::cout << "Plugin loaded successfully: " << info.name << std::endl;
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    PluginInfo& info = it->second;
    
    // 如果插件已启用，先禁用
    if (info.enabled) {
        disablePlugin(pluginName);
    }
    
    // 清理插件
    if (info.plugin) {
        info.plugin->cleanup();
    }
    
    // 卸载动态库
    unloadLibrary(info.libraryHandle);
    
    // 从列表中移除
    plugins_.erase(it);
    
    std::cout << "Plugin unloaded: " << pluginName << std::endl;
    return true;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    PluginInfo& info = it->second;
    if (info.enabled) {
        return true; // 已经启用
    }
    
    if (info.plugin) {
        info.plugin->enable();
        info.enabled = true;
        std::cout << "Plugin enabled: " << pluginName << std::endl;
        return true;
    }
    
    return false;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    PluginInfo& info = it->second;
    if (!info.enabled) {
        return true; // 已经禁用
    }
    
    if (info.plugin) {
        info.plugin->disable();
        info.enabled = false;
        std::cout << "Plugin disabled: " << pluginName << std::endl;
        return true;
    }
    
    return false;
}

std::vector<std::string> PluginManager::getPluginList() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

PluginInterface* PluginManager::getPlugin(const std::string& pluginName) const {
    auto it = plugins_.find(pluginName);
    return it != plugins_.end() ? it->second.plugin.get() : nullptr;
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    return plugins_.find(pluginName) != plugins_.end();
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    auto it = plugins_.find(pluginName);
    return it != plugins_.end() && it->second.enabled;
}

size_t PluginManager::scanPluginDirectory(const std::string& pluginDir) {
    size_t loadedCount = 0;
    
    try {
        if (!std::filesystem::exists(pluginDir)) {
            std::cout << "Plugin directory does not exist: " << pluginDir << std::endl;
            return 0;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(pluginDir)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                // 检查是否为动态库文件
#ifdef _WIN32
                if (extension == ".dll") {
#elif defined(__APPLE__)
                if (extension == ".dylib") {
#else
                if (extension == ".so") {
#endif
                    if (loadPlugin(path)) {
                        loadedCount++;
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error scanning plugin directory: " << e.what() << std::endl;
    }
    
    std::cout << "Scanned plugin directory: " << pluginDir << ", loaded " << loadedCount << " plugins" << std::endl;
    return loadedCount;
}

bool PluginManager::savePluginConfig(const std::string& configPath) {
    // TODO: 实现插件配置保存
    // 这里可以保存插件的启用/禁用状态等信息
    return true;
}

bool PluginManager::loadPluginConfig(const std::string& configPath) {
    // TODO: 实现插件配置加载
    // 这里可以加载插件的启用/禁用状态等信息
    return true;
}

void* PluginManager::loadLibrary(const std::string& pluginPath) {
#ifdef _WIN32
    return LoadLibraryA(pluginPath.c_str());
#else
    return dlopen(pluginPath.c_str(), RTLD_LAZY);
#endif
}

void PluginManager::unloadLibrary(void* handle) {
    if (!handle) return;
    
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
}

PluginCreateFunc PluginManager::getCreateFunction(void* handle) {
    if (!handle) return nullptr;
    
#ifdef _WIN32
    return reinterpret_cast<PluginCreateFunc>(GetProcAddress(static_cast<HMODULE>(handle), "createPlugin"));
#else
    return reinterpret_cast<PluginCreateFunc>(dlsym(handle, "createPlugin"));
#endif
}

PluginDestroyFunc PluginManager::getDestroyFunction(void* handle) {
    if (!handle) return nullptr;
    
#ifdef _WIN32
    return reinterpret_cast<PluginDestroyFunc>(GetProcAddress(static_cast<HMODULE>(handle), "destroyPlugin"));
#else
    return reinterpret_cast<PluginDestroyFunc>(dlsym(handle, "destroyPlugin"));
#endif
}
