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

	void getPhysicalDevice();
	void createLogicalDevice();
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};

