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
    else ConsoleText::printError("Physical device surface formats count 0!");
    
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

static void createImageView(const VkDevice& device,
                            VkImageView* pImageView,
                            VkImage& image,
                            VkFormat format)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    
    if (vkCreateImageView(device, &createInfo, nullptr, pImageView) != VK_SUCCESS) {
        ConsoleText::printError("Failed to create texture image view!");
        std::exit(0);
    }
}

static VkFormat findSupportedFormat(const VkPhysicalDevice& physicalDevice,
                                    const std::vector<VkFormat> candidates,
                                    VkImageTiling tiling,
                                    VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);
        
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (properties.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                 (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
        
        ConsoleText::printError("Failed to find supported format!");
        std::exit(0);
    }
}

static VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice) {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    
    return findSupportedFormat(physicalDevice, candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
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
    ConsoleText::printError("Failed to find suitable memory type!");
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
        ConsoleText::printError("Failed to create buffer!");
        std::exit(0);
    }
    else ConsoleText::printGreen("Buffer was created!");
    
    
    VkMemoryRequirements memRequirnaments;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirnaments);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirnaments.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice,
                                               memRequirnaments.memoryTypeBits,
                                               properties);
    
    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        ConsoleText::printError("Failed to allocate buffer memory!");
        std::exit(0);
    }
    else ConsoleText::printGreen("Buffer memory was allocated!");
    
    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

}
