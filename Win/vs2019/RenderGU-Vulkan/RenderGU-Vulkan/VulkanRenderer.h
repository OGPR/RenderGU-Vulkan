#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include "Utilities.h"

struct VulkanRenderer
{
	void createInstance();
	int init(GLFWwindow* window);
	void cleanup();
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	


	GLFWwindow* window;
	VkInstance instance;

	struct
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;

	VkQueue graphicsQueue;
	const char* validationLayer = "VK_LAYER_KHRONOS_validation";





	void getPhysicalDevice();
	void createLogicalDevice();
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

	// Validation Layers code
	#ifdef NDEBUG
		const bool validationLayers = false;
	#else
		const bool validationLayers = true;
	#endif

	bool validationLayerSupport();

};

