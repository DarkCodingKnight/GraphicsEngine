#include "VulkanResources.hpp"

namespace Engine {

void VulkanResources::initVulkan(const char* appName, GLFWwindow* pGLFWwindow, bool enableValidationLayers) {
    this->pGLFWwindow = pGLFWwindow;
    this->enableValidationLayers = enableValidationLayers;
    validationLayers = ValidationLayers();
    
    createInstance(appName);
    checkInstanceExtensionSupport();
    
    if (enableValidationLayers) {
        validationLayers.checkValidationLayerSupport();
        validationLayers.createDebugUtilsMessenger(instance, nullptr);
    }
}

void VulkanResources::createInstance(const char* appName) {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = appName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;
    applicationInfo.pNext = nullptr;
    
    std::vector<const char*> GLFWextensions = getGLFWExtensions();
    GLFWextensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    GLFWextensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    GLFWextensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    if (enableValidationLayers) GLFWextensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instanceCreateInfo.enabledExtensionCount = (uint32_t)GLFWextensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = GLFWextensions.data();
    
    if (enableValidationLayers) {
        validationLayers.setupDebugMessenger();
        instanceCreateInfo.pNext = &(validationLayers.debugMessengerCreateInfo);
    }
    
    VkResult instanceCreationResult = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    
    if (instanceCreationResult != VK_SUCCESS) {
        switch(instanceCreationResult) {
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
        std::exit(0);
    }
    ConsoleText::printGreen("Instance was created!", "Vulkan");
}

std::vector<const char*> VulkanResources::getGLFWExtensions() {
    std::vector<const char*> extensions;
    
    uint32_t glfwExtensionsCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    
    for(uint32_t i = 0; i < glfwExtensionsCount; i++) {
        extensions.emplace_back(glfwExtensions[i]);
    }
    
    return extensions;
}

void VulkanResources::checkInstanceExtensionSupport() {
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

void VulkanResources::createSurface(GLFWwindow* pGLFWwindow) {
    if (glfwCreateWindowSurface(instance, pGLFWwindow, nullptr, &surface) != VK_SUCCESS) {
        ConsoleText::print("Failed to create surface!", "Vulkan");
        std::exit(0);
    }
    else ConsoleText::printGreen("Surface was created!", "Vulkan");
}

void VulkanResources::pickPhysicalDevice() {
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
    
    if (physicalDevice == VK_NULL_HANDLE) {
        ConsoleText::printError("Failed to find a suitable GPU!", "Vulkan");
        std::exit(0);
    }
    else ConsoleText::printGreen("Suitable GPU was founded!", "Vulkan");
}

bool VulkanResources::isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    
    if (indices.isComplete() && checkDeviceExtensionSupport(device)) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        std::string APIversion = "\tAPI version: " + std::to_string(deviceProperties.apiVersion);
        std::string diverVersion = "\tDriver version: " + std::to_string(deviceProperties.driverVersion);
        std::string deviceName = "\tGPU name: " + (std::string)(deviceProperties.deviceName);
        
        ConsoleText::printGreen("Device properties: ", "Vulkan");
        ConsoleText::printGreen(APIversion);
        ConsoleText::printGreen(diverVersion);
        ConsoleText::printGreen(deviceName);
        
        SwapChainSupportDetails swapChainSupport = getSwapChainSupport(device, surface);
        if (!swapChainSupport.formats.empty() &&
            !swapChainSupport.presentModes.empty()) return true;
        else return false;
    }
    else return false;
}

bool VulkanResources::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
}

void VulkanResources::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo logicalDeviceCreateInfo{};
    logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    //FOR WINDOWS & LINUX
    //if (enableValidationLayers) {
    //    logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    //    logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    //}
    //else logicalDeviceCreateInfo.enabledLayerCount = 0;
    
    if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        ConsoleText::printError("Filed to create logical device!", "Vulkan");
        std::exit(0);
    }
    else ConsoleText::printGreen("Logical device was created!", "Vulkan");
    
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanResources::cleanUp() {
    vkDestroyDevice(logicalDevice, nullptr);
    ConsoleText::printGreen("Logical device was Destroyed!", "Vulkan");
    
    if (enableValidationLayers) {
        validationLayers.destroyDebuUtilsMessenger(instance, nullptr);
    }
    
    vkDestroySurfaceKHR(instance, surface, nullptr);
    ConsoleText::printGreen("Surface was destroyed!", "Vulkan");
    
    vkDestroyInstance(instance, nullptr);
    ConsoleText::printGreen("Instance was destroyed!", "Vulkan");
}

}
