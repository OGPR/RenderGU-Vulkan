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
	int Init(GLFWwindow*);
	void CreateInstance();
	void SetupDebugMessenger();
	void CreateSurface();
	void GetPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapchain();
	void CreateImageViews();
	void CreateGraphicsPipeline();

	void DestroyDebugMessenger();
	void Cleanup();
	bool CheckInstanceExtensionSupport(std::vector<const char*>*);
	bool CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice, const std::vector<const char*>&);
	bool CheckDeviceSuitable(VkPhysicalDevice);
	bool ValidationLayerSupport();
	
	QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice);
	SwapchainDesc CreateSwapchainDesc(VkPhysicalDevice);

	// Validation Layers code
	#ifdef NDEBUG
		const bool validationLayers = false;
	#else
		const bool validationLayers = true;
	#endif

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*,
		void*);


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
	uint32_t SwapchainImageCount = 0;

	VulkanValidationDesiredMsgSeverity* pVulkanValidationDesiredMsgSeverity = nullptr;
};

