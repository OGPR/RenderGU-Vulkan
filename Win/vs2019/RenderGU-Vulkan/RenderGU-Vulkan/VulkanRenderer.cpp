#include "VulkanRenderer.h"
#include <iostream>

int VulkanRenderer::init(GLFWwindow* window)
{
	this->window = window;

	createInstance();
	getPhysicalDevice();

	return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroyInstance(this->instance, nullptr);
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	// --- Begin get extension count ---
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// --- End get extension count ---

	// --- Begin create extension list, using count from above ---
	std::vector<VkExtensionProperties> extensionList(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionList.data());
	// --- End create extension list, using count from above ---

	// --- Begin if given extensions are in list of available ones --- 
	for (const VkExtensionProperties& foundExtension : extensionList)
	{
		bool hasExtension = false;
		for (const char* givenExtension : *checkExtensions)
		{
			if (strcmp(givenExtension, foundExtension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
			return false;
	}

	return true;
	// --- End if given extensions are in list of available ones --- 
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	// --- Placeholders for now ---
	// Information about the device
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Information about what the device can do
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	// --- End placeholders --- 

	// Queue Information
	QueueFamilyIndices indices = getQueueFamilies(device);
	
	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// TODO this pattern is familiar enough that it _might_ be worth abstracting(?)
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each of queue family and check if at least one of type of require queue
	int queueIndex  = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilyList)
	{
		++queueIndex;
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = queueIndex;
			break;
		}
	}

	return indices;
}

void VulkanRenderer::getPhysicalDevice()
{
	// Enumerate Physical Devices the vkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr);

	if (!deviceCount)
		throw std::runtime_error("Can't find GPUs that support our Vulkan Instance!");

	// Get list of physical devices (using newly populated deviceCount
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(this->instance, &deviceCount, deviceList.data());

	// Find valid device
	for (const VkPhysicalDevice& physicalDevice : deviceList)
	{
		if (checkDeviceSuitable(physicalDevice))
		{
			mainDevice.physicalDevice = physicalDevice;
			break;
		}
	}

}

void VulkanRenderer::createInstance()
{
	// ---- Begin AppInfo ----
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "RenderGU-VulkanApp";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;
	// ---- End AppInfo ----


	// ---- Begin CreateInfo ----
	std::vector<const char*> instanceExtensionList;
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionArray = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensionList.push_back(glfwExtensionArray[i]);
	}

	if (!checkInstanceExtensionSupport(&instanceExtensionList))
	{
		throw std::runtime_error("We don't have the required instance extensions!");
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionList.size());
	createInfo.ppEnabledExtensionNames = instanceExtensionList.data();
	// ---- End CreateInfo ----

	// ---- Begin CreateInstance ----
	if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS)
	{
		throw std::runtime_error("vkCreateInstance(...) did not return success");
	}

	// ---- End CreateInstance ----

}
