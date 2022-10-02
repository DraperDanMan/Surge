#pragma once

#include "NodeCanvas.h"

#include <string>
#include <functional>

#include "ExplorerWindow.h"
#include "imgui.h"
#include "OutputWindow.h"
#include "vulkan/vulkan.h"



struct GLFWwindow;

void check_vk_result(VkResult err);

namespace Surge
{

enum class FontType
{
    Regular,
    Bold,
    Header,
    Title,
};
    
    
struct AppConfig
{
    std::string name = "Surge";
    int width = 1440;
    int height = 900;
    std::string explorerRoot;
};

class Application
{
public:
    Application(const AppConfig& applicationSpecification = AppConfig());
    ~Application();

    void Run();
    void Close();

    static VkInstance GetInstance();
    static VkPhysicalDevice GetPhysicalDevice();
    static VkDevice GetDevice();

    static VkCommandBuffer GetCommandBuffer();
    static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

    static VkCommandBuffer GetComputeCommandBuffer();
    static void FlushComputeCommandBuffer(VkCommandBuffer commandBuffer);

    static void SubmitResourceFree(std::function<void()>&& func);

    static AppConfig& GetConfig();
    static ExplorerWindow* GetExplorer();

    static ImFont* GetFont(FontType font);
    
private:
    void Init();
    void Shutdown();
    void TryLoadAppConfig();
    void SaveAppConfig();

    //MenuBar
    void HandleAppMenuBar();
private:
    AppConfig m_config;
    GLFWwindow* m_windowHandle = nullptr;
    bool m_running = false;

    NodeCanvas *m_nodeCanvas = nullptr;
    ExplorerWindow *m_explorerWindow = nullptr;
    OutputWindow *m_outputWindow = nullptr;
};

}