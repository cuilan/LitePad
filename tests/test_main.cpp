#include <iostream>
#include <cassert>
#include <memory>
#include "../src/Editor.h"
#include "../src/ConfigManager.h"

/**
 * 简单的测试框架
 */
class TestFramework {
public:
    static void runTest(const std::string& testName, std::function<bool()> testFunc) {
        std::cout << "Running test: " << testName << "... ";
        
        try {
            bool result = testFunc();
            if (result) {
                std::cout << "PASSED" << std::endl;
            } else {
                std::cout << "FAILED" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "FAILED with exception: " << e.what() << std::endl;
        }
    }
    
    static void runAllTests() {
        std::cout << "=== Running LitePad Tests ===" << std::endl;
        
        // 编辑器测试
        testEditorBasic();
        testEditorFileOperations();
        testEditorTextOperations();
        
        // 配置管理器测试
        testConfigManagerBasic();
        testConfigManagerTypes();
        
        std::cout << "=== All tests completed ===" << std::endl;
    }

private:
    static void testEditorBasic() {
        std::cout << "\n--- Editor Basic Tests ---" << std::endl;
        
        runTest("Editor Constructor", []() {
            auto editor = std::make_unique<Editor>();
            return editor != nullptr;
        });
        
        runTest("Editor Initial State", []() {
            auto editor = std::make_unique<Editor>();
            return editor->getContent().empty() && 
                   editor->getFilePath().empty() && 
                   !editor->isModified();
        });
        
        runTest("Editor Line Count", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Line 1\nLine 2\nLine 3");
            return editor->getLineCount() == 3;
        });
    }
    
    static void testEditorFileOperations() {
        std::cout << "\n--- Editor File Operations Tests ---" << std::endl;
        
        runTest("Editor Set Content", []() {
            auto editor = std::make_unique<Editor>();
            std::string testContent = "Test content";
            editor->setContent(testContent);
            return editor->getContent() == testContent && editor->isModified();
        });
        
        runTest("Editor Clear", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Some content");
            editor->clear();
            return editor->getContent().empty() && !editor->isModified();
        });
    }
    
    static void testEditorTextOperations() {
        std::cout << "\n--- Editor Text Operations Tests ---" << std::endl;
        
        runTest("Editor Insert Text", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Hello World");
            editor->insertText(5, " Beautiful ");
            return editor->getContent() == "Hello Beautiful World";
        });
        
        runTest("Editor Delete Text", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Hello Beautiful World");
            editor->deleteText(5, 10);
            return editor->getContent() == "Hello World";
        });
        
        runTest("Editor Find Text", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Hello World Hello");
            size_t pos = editor->findText("Hello", 0, true);
            return pos == 0;
        });
        
        runTest("Editor Replace Text", []() {
            auto editor = std::make_unique<Editor>();
            editor->setContent("Hello World Hello");
            size_t count = editor->replaceText("Hello", "Hi", 0, true);
            return count == 2 && editor->getContent() == "Hi World Hi";
        });
    }
    
    static void testConfigManagerBasic() {
        std::cout << "\n--- Config Manager Basic Tests ---" << std::endl;
        
        runTest("Config Manager Constructor", []() {
            auto config = std::make_unique<ConfigManager>();
            return config != nullptr;
        });
        
        runTest("Config Manager Set/Get String", []() {
            auto config = std::make_unique<ConfigManager>();
            config->setString("test.key", "test.value");
            return config->getString("test.key") == "test.value";
        });
        
        runTest("Config Manager Default Value", []() {
            auto config = std::make_unique<ConfigManager>();
            return config->getString("nonexistent.key", "default") == "default";
        });
    }
    
    static void testConfigManagerTypes() {
        std::cout << "\n--- Config Manager Type Tests ---" << std::endl;
        
        runTest("Config Manager Int", []() {
            auto config = std::make_unique<ConfigManager>();
            config->setInt("test.int", 42);
            return config->getInt("test.int") == 42;
        });
        
        runTest("Config Manager Bool", []() {
            auto config = std::make_unique<ConfigManager>();
            config->setBool("test.bool", true);
            return config->getBool("test.bool") == true;
        });
        
        runTest("Config Manager Double", []() {
            auto config = std::make_unique<ConfigManager>();
            config->setDouble("test.double", 3.14);
            return config->getDouble("test.double") == 3.14;
        });
        
        runTest("Config Manager Has Key", []() {
            auto config = std::make_unique<ConfigManager>();
            config->setString("test.key", "value");
            return config->hasKey("test.key") && !config->hasKey("nonexistent.key");
        });
    }
};

/**
 * 主函数
 */
int main() {
    try {
        TestFramework::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test framework error: " << e.what() << std::endl;
        return 1;
    }
}
