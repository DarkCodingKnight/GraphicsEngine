#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <string>
#include <set>

#include "ConsoleText.hpp"

namespace Engine {

const std::vector<const char*> validationLayerExtensions = {
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

class ValidationLayers {
public:
    ValidationLayers() {};
    
    bool checkValidationLayerSupport();
    void setupDebugMessenger();
    void createDebugUtilsMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator);
    void destroyDebuUtilsMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator);
    
    ~ValidationLayers() {};
    
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
};

}
