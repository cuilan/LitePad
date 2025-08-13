#ifndef LINE_NUMBER_PLUGIN_H
#define LINE_NUMBER_PLUGIN_H

#include "../src/PluginInterface.h"
#include <memory>

class Editor;

/**
 * 行号插件类
 * 在编辑器左侧显示行号
 */
class LineNumberPlugin : public PluginInterface {
public:
    LineNumberPlugin();
    ~LineNumberPlugin() override;
    
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
     * 更新行号显示
     */
    void updateLineNumbers();
    
    /**
     * 设置行号显示样式
     * @param fontFamily 字体族
     * @param fontSize 字体大小
     * @param fontColor 字体颜色
     */
    void setLineNumberStyle(const std::string& fontFamily, int fontSize, const std::string& fontColor);
    
    /**
     * 设置行号背景颜色
     * @param backgroundColor 背景颜色
     */
    void setLineNumberBackground(const std::string& backgroundColor);
    
    /**
     * 设置行号对齐方式
     * @param alignment 对齐方式（left, center, right）
     */
    void setLineNumberAlignment(const std::string& alignment);
    
    /**
     * 显示/隐藏当前行高亮
     * @param show 是否显示
     */
    void setCurrentLineHighlight(bool show);
    
    /**
     * 设置行号区域宽度
     * @param width 宽度（像素）
     */
    void setLineNumberWidth(int width);
    
    /**
     * 获取行号区域宽度
     * @return 宽度（像素）
     */
    int getLineNumberWidth() const;

private:
    std::shared_ptr<Editor> editor_;
    bool enabled_;
    bool currentLineHighlight_;
    
    // 样式设置
    std::string fontFamily_;
    int fontSize_;
    std::string fontColor_;
    std::string backgroundColor_;
    std::string alignment_;
    int lineNumberWidth_;
    
    /**
     * 绘制行号
     */
    void drawLineNumbers();
    
    /**
     * 计算行号区域
     */
    void calculateLineNumberArea();
    
    /**
     * 处理滚动事件
     */
    void handleScroll();
    
    /**
     * 处理选择变化事件
     */
    void handleSelectionChanged();
    
    /**
     * 处理内容变化事件
     */
    void handleContentChanged();
};

#endif // LINE_NUMBER_PLUGIN_H
