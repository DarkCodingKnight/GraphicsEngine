#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <string>
#include <set>

#include "ConsoleText.hpp"

namespace Engine {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

const std::vector<const char*> deviceExtensions = {
    //"VK_KHR_swapchain",
    "VK_KHR_portability_subset",
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) {
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            std::string message = "Diagnostic message: " + (std::string)(pCallbackData->pMessage);
            ConsoleText::printWhite(message, "Debug messenger");
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            std::string message = "Info message: " + (std::string)(pCallbackData->pMessage);
            ConsoleText::printWhite(message, "Debug messenger");
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            std::string message = "WARNING message: " + (std::string)(pCallbackData->pMessage);
            ConsoleText::printPurple(message, "Debug messenger");
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            std::string message = "ERROR message: " + (std::string)(pCallbackData->pMessage);
            ConsoleText::printRed(message, "Debug messenger");
            break;
        }
        default:
            ConsoleText::print((std::string)(pCallbackData->pMessage), "Debug messenger");;
    }
}

class Vulkan {
public:
    Vulkan() {};
    
    void initVulkan(const char* appName,
                    bool enableValidationLayers);
    void createInstance(const char* appName);
    void checkInstanceExtensionSupport();
    
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    
    void createLogicalDevice();
    
    //Validation layers
    void setupDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo);
    bool checkValidationLayerSupport();
    void createDebugUtilsMessenger(VkInstance instance,
                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                   const VkAllocationCallbacks* pAllocator,
                                   VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebuUtilsMessenger(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);
    void cleanUp();
    
    std::vector<const char*> getGLFWExtensions();
    
    ~Vulkan() {};
    
private:
    bool enableValidationLayers = false;
    
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
};

}
