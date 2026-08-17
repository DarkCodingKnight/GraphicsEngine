#include "ValidationLayers.hpp"

namespace Engine {

bool ValidationLayers::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayerExtensions) {
        bool layerFound = false;
        for(const auto& layerProperties : availableLayers) {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            ConsoleText::printGreen("Validation layers were not found!", "ValidationLayers");
            return false;
        }
    }
    
    std::string message = "Validation layers are founded! Layers count: " + std::to_string(layerCount);
    ConsoleText::printGreen(message, "ValidationLayers");
    for(const auto& layerProperties : availableLayers) {
        std::string message = "\t" + (std::string)(layerProperties.layerName);
        ConsoleText::printGreen(message);
    }
    
    return true;
}

void ValidationLayers::createDebugUtilsMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, &debugMessengerCreateInfo, pAllocator, &debugMessenger);
        ConsoleText::printGreen("Debug utils messenger was created!", "ValidationLayers");
    }
    else {
        ConsoleText::printError("Debug utils messenger was not created!", "ValidationLayers");
    }
}

void ValidationLayers::setupDebugMessenger() {
    debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = debugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;
    debugMessengerCreateInfo.pNext = nullptr;
}

void ValidationLayers::destroyDebuUtilsMessenger(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        ConsoleText::printGreen("Debug utils messenger was destroyed!", "ValidationLayers");
        func(instance, debugMessenger, pAllocator);
    }
    else {
        ConsoleText::printError("Debug utils messenger was not destroyed!", "ValidationLayers");
    }
}


}
