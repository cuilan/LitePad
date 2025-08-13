#ifndef SYNTAX_HIGHLIGHT_PLUGIN_H
#define SYNTAX_HIGHLIGHT_PLUGIN_H

#include "../src/PluginInterface.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Editor;

/**
 * 语法高亮插件类
 * 为不同编程语言提供语法高亮功能
 */
class SyntaxHighlightPlugin : public PluginInterface {
public:
    SyntaxHighlightPlugin();
    ~SyntaxHighlightPlugin() override;
    
    // 实现插件接口
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    bool initialize() override;
    void cleanup() override;
    void enable() override;
    void disable() override;
    bool isEnabled() const override;
    
    /**
     * 设置编辑器实例
     * @param editor 编辑器指针
     */
    void setEditor(std::shared_ptr<Editor> editor);
    
    /**
     * 获取编辑器实例
     * @return 编辑器指针
     */
    std::shared_ptr<Editor> getEditor() const;
    
    /**
     * 设置编程语言
     * @param language 语言名称
     */
    void setLanguage(const std::string& language);
    
    /**
     * 获取当前编程语言
     * @return 语言名称
     */
    std::string getLanguage() const;
    
    /**
     * 获取支持的语言列表
     * @return 语言列表
     */
    std::vector<std::string> getSupportedLanguages() const;
    
    /**
     * 应用语法高亮
     */
    void applyHighlighting();
    
    /**
     * 清除语法高亮
     */
    void clearHighlighting();
    
    /**
     * 设置关键字颜色
     * @param color 颜色值
     */
    void setKeywordColor(const std::string& color);
    
    /**
     * 设置字符串颜色
     * @param color 颜色值
     */
    void setStringColor(const std::string& color);
    
    /**
     * 设置注释颜色
     * @param color 颜色值
     */
    void setCommentColor(const std::string& color);
    
    /**
     * 设置数字颜色
     * @param color 颜色值
     */
    void setNumberColor(const std::string& color);
    
    /**
     * 设置操作符颜色
     * @param color 颜色值
     */
    void setOperatorColor(const std::string& color);
    
    /**
     * 设置函数颜色
     * @param color 颜色值
     */
    void setFunctionColor(const std::string& color);
    
    /**
     * 设置类名颜色
     * @param color 颜色值
     */
    void setClassNameColor(const std::string& color);
    
    /**
     * 启用/禁用特定语言的高亮
     * @param language 语言名称
     * @param enabled 是否启用
     */
    void setLanguageEnabled(const std::string& language, bool enabled);
    
    /**
     * 检查语言是否启用
     * @param language 语言名称
     * @return 是否启用
     */
    bool isLanguageEnabled(const std::string& language) const;
    
    /**
     * 添加自定义关键字
     * @param language 语言名称
     * @param keywords 关键字列表
     */
    void addCustomKeywords(const std::string& language, const std::vector<std::string>& keywords);
    
    /**
     * 移除自定义关键字
     * @param language 语言名称
     * @param keywords 关键字列表
     */
    void removeCustomKeywords(const std::string& language, const std::vector<std::string>& keywords);

private:
    std::shared_ptr<Editor> editor_;
    bool enabled_;
    std::string currentLanguage_;
    
    // 颜色设置
    std::string keywordColor_;
    std::string stringColor_;
    std::string commentColor_;
    std::string numberColor_;
    std::string operatorColor_;
    std::string functionColor_;
    std::string classNameColor_;
    
    // 语言支持
    std::unordered_map<std::string, bool> languageSupport_;
    std::unordered_map<std::string, std::vector<std::string>> customKeywords_;
    
    /**
     * 检测文件类型
     * @param filePath 文件路径
     * @return 检测到的语言
     */
    std::string detectLanguage(const std::string& filePath);
    
    /**
     * 解析语法规则
     * @param language 语言名称
     */
    void parseSyntaxRules(const std::string& language);
    
    /**
     * 高亮关键字
     */
    void highlightKeywords();
    
    /**
     * 高亮字符串
     */
    void highlightStrings();
    
    /**
     * 高亮注释
     */
    void highlightComments();
    
    /**
     * 高亮数字
     */
    void highlightNumbers();
    
    /**
     * 高亮操作符
     */
    void highlightOperators();
    
    /**
     * 高亮函数
     */
    void highlightFunctions();
    
    /**
     * 高亮类名
     */
    void highlightClassNames();
    
    /**
     * 处理内容变化事件
     */
    void handleContentChanged();
    
    /**
     * 处理文件路径变化事件
     */
    void handleFilePathChanged();
    
    /**
     * 加载语言配置文件
     * @param language 语言名称
     */
    void loadLanguageConfig(const std::string& language);
    
    /**
     * 保存语言配置
     * @param language 语言名称
     */
    void saveLanguageConfig(const std::string& language);
};

#endif // SYNTAX_HIGHLIGHT_PLUGIN_H
