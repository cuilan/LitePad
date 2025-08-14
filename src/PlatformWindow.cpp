#include "PlatformWindow.h"

// 包含平台特定的头文件
#ifdef MACOS
#include "platform/macos/MacOSWindow.h"
#elif defined(WIN32)
#include "platform/windows/WindowsWindow.h"
#elif defined(LINUX)
#include "platform/linux/LinuxWindow.h"
#endif

std::unique_ptr<PlatformWindow> PlatformWindowFactory::createWindow() {
#ifdef MACOS
    return std::make_unique<MacOSWindow>();
#elif defined(WIN32)
    return std::make_unique<WindowsWindow>();
#elif defined(LINUX)
    return std::make_unique<LinuxWindow>();
#else
    #error "Unsupported platform"
#endif
}
