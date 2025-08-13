#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * 编辑器类
 * 提供基础的文本编辑功能
 */
class Editor {
public:
    Editor();
    ~Editor();
    
    /**
     * 打开文件
     * @param filePath 文件路径
     * @return 是否打开成功
     */
    bool openFile(const std::string& filePath);
    
    /**
     * 保存文件
     * @param filePath 文件路径，如果为空则保存到当前文件
     * @return 是否保存成功
     */
    bool saveFile(const std::string& filePath = "");
    
    /**
     * 另存为
     * @param filePath 新文件路径
     * @return 是否保存成功
     */
    bool saveAs(const std::string& filePath);
    
    /**
     * 获取文件内容
     * @return 文件内容
     */
    std::string getContent() const;
    
    /**
     * 设置文件内容
     * @param content 新内容
     */
    void setContent(const std::string& content);
    
    /**
     * 获取当前文件路径
     * @return 文件路径
     */
    std::string getFilePath() const;
    
    /**
     * 检查文件是否已修改
     * @return 是否已修改
     */
    bool isModified() const;
    
    /**
     * 设置修改状态
     * @param modified 修改状态
     */
    void setModified(bool modified);
    
    /**
     * 获取行数
     * @return 总行数
     */
    size_t getLineCount() const;
    
    /**
     * 获取指定行的内容
     * @param lineNumber 行号（从1开始）
     * @return 行内容
     */
    std::string getLine(size_t lineNumber) const;
    
    /**
     * 插入文本
     * @param position 插入位置
     * @param text 要插入的文本
     */
    void insertText(size_t position, const std::string& text);
    
    /**
     * 删除文本
     * @param start 开始位置
     * @param length 删除长度
     */
    void deleteText(size_t start, size_t length);
    
    /**
     * 查找文本
     * @param searchText 要查找的文本
     * @param startPosition 开始查找位置
     * @param caseSensitive 是否区分大小写
     * @return 找到的位置，如果未找到则返回std::string::npos
     */
    size_t findText(const std::string& searchText, size_t startPosition = 0, bool caseSensitive = true);
    
    /**
     * 替换文本
     * @param searchText 要查找的文本
     * @param replaceText 替换文本
     * @param startPosition 开始位置
     * @param caseSensitive 是否区分大小写
     * @return 替换次数
     */
    size_t replaceText(const std::string& searchText, const std::string& replaceText, 
                      size_t startPosition = 0, bool caseSensitive = true);
    
    /**
     * 撤销操作
     * @return 是否撤销成功
     */
    bool undo();
    
    /**
     * 重做操作
     * @return 是否重做成功
     */
    bool redo();
    
    /**
     * 检查是否可以撤销
     * @return 是否可以撤销
     */
    bool canUndo() const;
    
    /**
     * 检查是否可以重做
     * @return 是否可以重做
     */
    bool canRedo() const;
    
    /**
     * 清空内容
     */
    void clear();
    
    /**
     * 设置内容变化回调
     * @param callback 回调函数
     */
    void setContentChangedCallback(std::function<void()> callback);
    
    /**
     * 设置文件路径变化回调
     * @param callback 回调函数
     */
    void setFilePathChangedCallback(std::function<void(const std::string&)> callback);

private:
    std::string content_;
    std::string filePath_;
    bool modified_;
    std::vector<std::string> undoStack_;
    std::vector<std::string> redoStack_;
    std::function<void()> contentChangedCallback_;
    std::function<void(const std::string&)> filePathChangedCallback_;
    
    /**
     * 更新撤销重做栈
     */
    void updateUndoRedoStack();
    
    /**
     * 通知内容变化
     */
    void notifyContentChanged();
    
    /**
     * 通知文件路径变化
     */
    void notifyFilePathChanged();
};

#endif // EDITOR_H
