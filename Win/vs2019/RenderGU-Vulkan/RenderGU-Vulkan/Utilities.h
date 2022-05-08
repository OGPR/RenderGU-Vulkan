#pragma once
#include <vector>

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

