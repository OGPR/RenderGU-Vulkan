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

struct VulkanRenderer
{
	VulkanRenderer()
	{

	}

	VulkanRenderer(VulkanValidationDesiredMsgSeverity* _vulkanValidationDesiredMsgSeverityPtr)
		:pVulkanValidationDesiredMsgSeverity(_vulkanValidationDesiredMsgSeverityPtr)
	{
	}
	void createInstance();
	int init(GLFWwindow* window);
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

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "Custom callback : Validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
		//return VK_TRUE;
	}

	void setupDebugMessenger()
	{
		if (!this->validationLayers)
			return;
		
		VkDebugUtilsMessengerCreateInfoEXT createDebugMsgInfo = {};
		createDebugMsgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		
		// Specify the types of severity to call the debug callback for
		if (!this->pVulkanValidationDesiredMsgSeverity)
		{
			throw std::runtime_error("Our vulkan validation desired msg severity type ptr "
				"is not expected to be null at this point - check renderer initialisation");
		}

		createDebugMsgInfo.messageSeverity =
			this->pVulkanValidationDesiredMsgSeverity->State;

		createDebugMsgInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createDebugMsgInfo.pfnUserCallback = debugCallback;
		createDebugMsgInfo.pUserData = nullptr;

		if (!this->instance)
		{
			throw std::runtime_error("Instance has yet to be created!");
		}
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			this->instance, "vkCreateDebugUtilsMessengerEXT");

		if (!func)
		{
			throw std::runtime_error("vk extension not present");
		}

		func(this->instance, &createDebugMsgInfo, nullptr, &debugMessenger);

	}
	
	void destroyDebugMessenger()
	{
		if (!this->instance)
			throw std::runtime_error("No instance has been created!");
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			this->instance, "vkDestroyDebugUtilsMessengerEXT");

		if (!func)
		{
			throw std::runtime_error("vk extension not present");
		}

		func(this->instance, debugMessenger, nullptr);
	}




	void getPhysicalDevice();
	void createLogicalDevice();
	QueueFamilyIndices getQueueFamilyIndices(VkPhysicalDevice device);
	SwapChainDesc CreateSwapChainDesc(VkPhysicalDevice);
	void createSurface();

	// Validation Layers code
	#ifdef NDEBUG
		const bool validationLayers = false;
	#else
		const bool validationLayers = true;
	#endif

	bool validationLayerSupport();

	VulkanValidationDesiredMsgSeverity* pVulkanValidationDesiredMsgSeverity = nullptr;
};

