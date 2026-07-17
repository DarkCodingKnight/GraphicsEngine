#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "ConsoleText.hpp"

namespace Engine {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

static SwapChainSupportDetails getSwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
    SwapChainSupportDetails swapChainDetails{};
    
    //Surface capabilities.
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &(swapChainDetails.capabilities));
    
    //Surface formats.
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        swapChainDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, (swapChainDetails.formats).data());
    }
    else ConsoleText::printError("Physical device surface formats count 0", "SwapChain");
    
    //Present modes.
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        swapChainDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, (swapChainDetails.presentModes).data());
    }
    
    return swapChainDetails;
}

static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int index = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        
        //Graphics queue support
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = index;
        }
        
        //Present queue support
        if (presentSupport) indices.presentFamily = index;
        
        //If present and graphics queues are supported -> break
        if (indices.isComplete()) break;
        index++;
    }
    
    return indices;
}

static uint32_t findMemoryType(const VkPhysicalDevice& device,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    ConsoleText::printError("Failed to find suitable memory type!", "Render program");
    std::exit(0);
}

static void createBufferSource(const VkDevice& logicalDevice,
                               const VkPhysicalDevice& physicalDevice,
                               VkDeviceSize size,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               VkBuffer& buffer,
                               VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        ConsoleText::printError("Failed to create buffer!", "Render program");
        std::exit(0);
    }
    else ConsoleText::printGreen("Buffer was created!", "Render program");
    
    
    VkMemoryRequirements memRequirnaments;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirnaments);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirnaments.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice,
                                               memRequirnaments.memoryTypeBits,
                                               properties);
    
    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate buffer memory!", "Render program");
        std::exit(0);
    }
    else ConsoleText::printGreen("Buffer memory was allocated!", "Render program");
    
    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

}
