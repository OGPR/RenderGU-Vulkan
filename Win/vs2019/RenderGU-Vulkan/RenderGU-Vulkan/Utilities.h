#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <fstream>

using RenderGU_BytecodeBuffer = std::vector<char>;

const std::vector<const char*> desiredPhyiscalDeviceExtenstions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (locations) of Queue Families
struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentationFamily = -1;

	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapchainDesc
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormatArray;
	std::vector<VkPresentModeKHR> presentationModeArray;

	bool isValid()
	{
		return std::size(surfaceFormatArray) && std::size(presentationModeArray);
	}
};

RenderGU_BytecodeBuffer ReadBytecode(const std::string Filename);
VkShaderModule CreateShaderModule(VkDevice LogicalDevice,
	RenderGU_BytecodeBuffer& BytecodeBuffer);

