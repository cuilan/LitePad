#include "Editor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

Editor::Editor() : modified_(false) {
    // 初始化编辑器
}

Editor::~Editor() = default;

bool Editor::openFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content_ = buffer.str();
        filePath_ = filePath;
        modified_ = false;
        
        // 更新撤销重做栈
        updateUndoRedoStack();
        
        // 通知文件路径变化
        notifyFilePathChanged();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Editor::saveFile(const std::string& filePath) {
    std::string targetPath = filePath.empty() ? filePath_ : filePath;
    if (targetPath.empty()) {
        return false;
    }
    
    try {
        std::ofstream file(targetPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << content_;
        file.close();
        
        if (filePath.empty()) {
            filePath_ = targetPath;
        }
        modified_ = false;
        
        // 通知文件路径变化
        notifyFilePathChanged();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Editor::saveAs(const std::string& filePath) {
    return saveFile(filePath);
}

std::string Editor::getContent() const {
    return content_;
}

void Editor::setContent(const std::string& content) {
    if (content_ != content) {
        content_ = content;
        modified_ = true;
        
        // 更新撤销重做栈
        updateUndoRedoStack();
        
        // 通知内容变化
        notifyContentChanged();
    }
}

std::string Editor::getFilePath() const {
    return filePath_;
}

bool Editor::isModified() const {
    return modified_;
}

void Editor::setModified(bool modified) {
    modified_ = modified;
}

size_t Editor::getLineCount() const {
    if (content_.empty()) {
        return 0;
    }
    
    size_t count = 1;
    for (char c : content_) {
        if (c == '\n') {
            count++;
        }
    }
    return count;
}

std::string Editor::getLine(size_t lineNumber) const {
    if (lineNumber < 1 || lineNumber > getLineCount()) {
        return "";
    }
    
    std::istringstream stream(content_);
    std::string line;
    size_t currentLine = 1;
    
    while (std::getline(stream, line) && currentLine < lineNumber) {
        currentLine++;
    }
    
    return line;
}

void Editor::insertText(size_t position, const std::string& text) {
    if (position > content_.length()) {
        position = content_.length();
    }
    
    content_.insert(position, text);
    modified_ = true;
    
    // 更新撤销重做栈
    updateUndoRedoStack();
    
    // 通知内容变化
    notifyContentChanged();
}

void Editor::deleteText(size_t start, size_t length) {
    if (start >= content_.length()) {
        return;
    }
    
    if (start + length > content_.length()) {
        length = content_.length() - start;
    }
    
    content_.erase(start, length);
    modified_ = true;
    
    // 更新撤销重做栈
    updateUndoRedoStack();
    
    // 通知内容变化
    notifyContentChanged();
}

size_t Editor::findText(const std::string& searchText, size_t startPosition, bool caseSensitive) {
    if (searchText.empty() || startPosition >= content_.length()) {
        return std::string::npos;
    }
    
    std::string searchContent = content_.substr(startPosition);
    std::string searchPattern = searchText;
    
    if (!caseSensitive) {
        std::transform(searchContent.begin(), searchContent.end(), searchContent.begin(), ::tolower);
        std::transform(searchPattern.begin(), searchPattern.end(), searchPattern.begin(), ::tolower);
    }
    
    size_t pos = searchContent.find(searchPattern);
    return pos == std::string::npos ? std::string::npos : startPosition + pos;
}

size_t Editor::replaceText(const std::string& searchText, const std::string& replaceText, 
                          size_t startPosition, bool caseSensitive) {
    size_t count = 0;
    size_t pos = startPosition;
    
    while ((pos = findText(searchText, pos, caseSensitive)) != std::string::npos) {
        deleteText(pos, searchText.length());
        insertText(pos, replaceText);
        pos += replaceText.length();
        count++;
    }
    
    return count;
}

bool Editor::undo() {
    if (undoStack_.empty()) {
        return false;
    }
    
    // 保存当前状态到重做栈
    redoStack_.push_back(content_);
    
    // 恢复上一个状态
    content_ = undoStack_.back();
    undoStack_.pop_back();
    modified_ = true;
    
    // 通知内容变化
    notifyContentChanged();
    
    return true;
}

bool Editor::redo() {
    if (redoStack_.empty()) {
        return false;
    }
    
    // 保存当前状态到撤销栈
    undoStack_.push_back(content_);
    
    // 恢复下一个状态
    content_ = redoStack_.back();
    redoStack_.pop_back();
    modified_ = true;
    
    // 通知内容变化
    notifyContentChanged();
    
    return true;
}

bool Editor::canUndo() const {
    return !undoStack_.empty();
}

bool Editor::canRedo() const {
    return !redoStack_.empty();
}

void Editor::clear() {
    content_.clear();
    filePath_.clear();
    modified_ = false;
    undoStack_.clear();
    redoStack_.clear();
    
    // 通知内容变化
    notifyContentChanged();
}

void Editor::setContentChangedCallback(std::function<void()> callback) {
    contentChangedCallback_ = callback;
}

void Editor::setFilePathChangedCallback(std::function<void(const std::string&)> callback) {
    filePathChangedCallback_ = callback;
}

void Editor::updateUndoRedoStack() {
    // 限制撤销栈大小
    const size_t maxStackSize = 100;
    
    if (undoStack_.size() >= maxStackSize) {
        undoStack_.erase(undoStack_.begin());
    }
    
    undoStack_.push_back(content_);
    
    // 清空重做栈
    redoStack_.clear();
}

void Editor::notifyContentChanged() {
    if (contentChangedCallback_) {
        contentChangedCallback_();
    }
}

void Editor::notifyFilePathChanged() {
    if (filePathChangedCallback_) {
        filePathChangedCallback_(filePath_);
    }
}
