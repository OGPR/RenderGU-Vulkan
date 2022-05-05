#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include "Utilities.h"
#include <iostream>

struct VulkanValidationDesiredMsgSeverity
{
	enum  MsgSeverity  
	{
		DIAG = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
		INFO = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
		WARN = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		ERROR = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
	};

	int State = 0;
};

struct DesiredSurfaceFormat
{
	// What our renderer will use
	static const  VkFormat Format = VK_FORMAT_B8G8R8A8_UNORM;
	static const VkColorSpaceKHR ColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
};

struct DesiredPresentation
{
	// What our renderer will use
	static const VkPresentModeKHR Mode = VK_PRESENT_MODE_MAILBOX_KHR;
};

struct RenderGU_Vk_Renderer
{
	RenderGU_Vk_Renderer()
	{

	}

	RenderGU_Vk_Renderer(VulkanValidationDesiredMsgSeverity* _vulkanValidationDesiredMsgSeverityPtr)
		:pVulkanValidationDesiredMsgSeverity(_vulkanValidationDesiredMsgSeverityPtr)
	{
	}
	int init(GLFWwindow* window);
	void createInstance();
	void cleanup();
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool checkPhysicalDeviceExtensionSupport(
		VkPhysicalDevice device,
		const std::vector<const char*>& desiredPhysicalDeviceExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	


	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	struct
	{
		VkPhysicalDevice physicalDevice = nullptr;
		VkDevice logicalDevice = nullptr;
	} mainDevice;

	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	const char* validationLayer = "VK_LAYER_KHRONOS_validation";
	VkSurfaceKHR surface = nullptr;
	VkSwapchainKHR swapchain;
	std::vector<VkImageView> ImageViewArray;
	uint32_t SwapChainImageCount = 0;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void setupDebugMessenger();
	
	void destroyDebugMessenger();

	void getPhysicalDevice();
	void createLogicalDevice();
	QueueFamilyIndices getQueueFamilyIndices(VkPhysicalDevice device);
	SwapChainDesc CreateSwapChainDesc(VkPhysicalDevice);
	void createSurface();
	void createSwapChain();
	void createImageViews();

	// Validation Layers code
	#ifdef NDEBUG
		const bool validationLayers = false;
	#else
		const bool validationLayers = true;
	#endif

	bool validationLayerSupport();

	VulkanValidationDesiredMsgSeverity* pVulkanValidationDesiredMsgSeverity = nullptr;
};

