#include "Vulkan_Init.hpp"

namespace Engine {

void Vulkan::initVulkan(const char* appName,
                        bool enableValidationLayers) {
    this->enableValidationLayers = enableValidationLayers;
    
    createInstance(appName);
    checkInstanceExtensionSupport();
}

void Vulkan::createInstance(const char* appName) {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = appName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;
    applicationInfo.pNext = nullptr;
    
    std::vector<const char*> extensions = getGLFWExtensions();
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    if (enableValidationLayers) {
        debugMessengerCreateInfo = {};
        setupDebugMessenger(&debugMessengerCreateInfo);
        instanceCreateInfo.pNext = &debugMessengerCreateInfo;
    }
    else {
        instanceCreateInfo.pNext = nullptr;
    }
    
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    
    if (result != VK_SUCCESS) {
        switch(result) {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                ConsoleText::printError("Instance was not created, out of host memory", "Vulkan");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                ConsoleText::printError("Instance was not created, out of device memory", "Vulkan");
                break;
            case VK_ERROR_INITIALIZATION_FAILED:
                ConsoleText::printError("Instance was not created, initialization failed", "Vulkan");
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                ConsoleText::printError("Instance was not created, layer not present", "Vulkan");
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                ConsoleText::printError("Instance was not created, extension not present", "Vulkan");
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                ConsoleText::printError("Instance was not created, Incompatible driver", "Vulkan");
                break;
            default:
                ConsoleText::printError("Instance was not created, unknown error", "Vulkan");
        }
        std:exit(0);
    }
    ConsoleText::printGreen("Instance was created!", "Vulkan");
    
    if (enableValidationLayers) {
        checkValidationLayerSupport();
        createDebugUtilsMessenger(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
    }
}

std::vector<const char*> Vulkan::getGLFWExtensions() {
    std::vector<const char*> extensions;
    
    uint32_t glfwExtensionsCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    
    for(uint32_t i = 0; i < glfwExtensionsCount; i++) {
        extensions.emplace_back(glfwExtensions[i]);
    }
    
    return extensions;
}

void Vulkan::checkInstanceExtensionSupport() {
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    
    ConsoleText::printGreen("Instance extensions: ", "Vulkan");
    for (uint32_t i = 0; i < extensionCount; i++) {
        const std::string message = "\t" + std::to_string(i + 1) + ". " + (std::string)(extensions.at(i).extensionName);
        ConsoleText::printGreen(message);
    }
}

bool Vulkan::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers) {
        bool layerFound = false;
        for(const auto& layerProperties : availableLayers) {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            ConsoleText::printGreen("Validation layers were not found!", "Vulkan");
            return false;
        }
    }
    
    std::string message = "Validation layers are founded! Layers count: " + std::to_string(layerCount);
    ConsoleText::printGreen(message, "Vulkan");
    for(const auto& layerProperties : availableLayers) {
        std::string message = "\t" + (std::string)(layerProperties.layerName);
        ConsoleText::printGreen(message);
    }
    
    return true;
}

void Vulkan::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    
    for (const auto& device : devices) {
        if(isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }
    
    if (physicalDevice == VK_NULL_HANDLE) ConsoleText::printError("Failed to find a suitable GPU!", "Vulkan");
}

bool Vulkan::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    std::string APIversion = "API version: " + std::to_string(deviceProperties.apiVersion);
    std::string diverVersion = "Driver version: " + std::to_string(deviceProperties.driverVersion);
    std::string deviceName = "GPU name: " + (std::string)(deviceProperties.deviceName);
    
    ConsoleText::printGreen("Device properties: ", "Vulkan");
    ConsoleText::printGreen(APIversion);
    ConsoleText::printGreen(diverVersion);
    ConsoleText::printGreen(deviceName);
    
    return true;
}

QueueFamilyIndices Vulkan::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    
    return indices;
}

void Vulkan::setupDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo) {
    (*debugMessengerCreateInfo).sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    (*debugMessengerCreateInfo).messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    (*debugMessengerCreateInfo).messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    (*debugMessengerCreateInfo).pfnUserCallback = debugCallback;
    (*debugMessengerCreateInfo).pUserData = nullptr;
    (*debugMessengerCreateInfo).pNext = nullptr;
}

void Vulkan::createDebugUtilsMessenger(VkInstance instance,
                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                   const VkAllocationCallbacks* pAllocator,
                                   VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        ConsoleText::printGreen("Debug utils messenger was created!", "Vulkan");
    }
    else {
        ConsoleText::printError("Debug utils messenger was not created!", "Vulkan");
    }
}

void Vulkan::destroyDebuUtilsMessenger(VkInstance instance,
                               VkDebugUtilsMessengerEXT debugMessenger,
                               const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        ConsoleText::printGreen("Debug utils messenger was destroyed!", "Vulkan");
        func(instance, debugMessenger, pAllocator);
    }
    else {
        ConsoleText::printError("Debug utils messenger was not destroyed!", "Vulkan");
    }
}

void Vulkan::cleanUp() {
    if (enableValidationLayers) {
        destroyDebuUtilsMessenger(instance, debugMessenger, nullptr);
    }
    
    vkDestroyInstance(instance, nullptr);
    ConsoleText::printGreen("Instance was destroyed!", "Vulkan");
}

}
