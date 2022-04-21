#include "VulkanRenderer.h"
#include <iostream>

int VulkanRenderer::init(GLFWwindow* window)
{
	this->window = window;

	createInstance();
	setupDebugMessenger();
	createSurface();
	getPhysicalDevice();
	createLogicalDevice();

	return 0;
}

void VulkanRenderer::cleanup()
{
	if (this->validationLayers)
	{
		destroyDebugMessenger();
	}
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
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
	QueueFamilyIndices indices = getQueueFamilyIndices(device);
	
	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilyIndices(VkPhysicalDevice device)
{
	QueueFamilyIndices queueFamilyIndices;

	// TODO this pattern is familiar enough that it _might_ be worth abstracting(?)
	uint32_t numQueueFamilies =  0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(numQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, queueFamilies.data());

	// Go through each queue family and check if at least one of type of required queue
	for (uint32_t i = 0; i < numQueueFamilies; ++i)
	{
		if (queueFamilies[i].queueCount > 0
			&& queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueFamilyIndices.graphicsFamily = i;
		}

		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentationSupport);
		if (queueFamilies[i].queueCount > 0 && presentationSupport)
		{
			queueFamilyIndices.presentationFamily = i;
		}

		if (queueFamilyIndices.isValid())
		{
			break;
		}
	}

	return queueFamilyIndices;
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

	if (!mainDevice.physicalDevice)
		throw std::runtime_error("No suitable physical device found");
}

void VulkanRenderer::createLogicalDevice()
{
	// Specify the queues the logical device needs to create
	QueueFamilyIndices queueIndicies = getQueueFamilyIndices(mainDevice.physicalDevice);
	if (!queueIndicies.isValid())
		throw std::runtime_error("Queue index not valid!");

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueIndicies.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};


	VkDeviceCreateInfo logicalDeviceCreateInfo = {};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = 1;
	logicalDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	logicalDeviceCreateInfo.enabledExtensionCount = 0; //device does not care about glfw (e.g), only instance does
	logicalDeviceCreateInfo.ppEnabledExtensionNames = nullptr;
	logicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

	VkResult result = vkCreateDevice(
		mainDevice.physicalDevice,
		&logicalDeviceCreateInfo,
		nullptr,
		&mainDevice.logicalDevice);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a logical device");
	}

	vkGetDeviceQueue(mainDevice.logicalDevice, queueIndicies.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::createInstance()
{
	if (validationLayer && !validationLayerSupport())
		throw std::runtime_error("Validation layers requested,"
			"but specified layer was not found");

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

	if (this->validationLayers)
	{
		instanceExtensionList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!checkInstanceExtensionSupport(&instanceExtensionList))
	{
		throw std::runtime_error("We don't have the required instance extensions!");
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = this->validationLayers ? 1 : 0;
	createInfo.ppEnabledLayerNames = this->validationLayers ?  &this->validationLayer : nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionList.size());
	createInfo.ppEnabledExtensionNames = instanceExtensionList.data();

	VkDebugUtilsMessengerCreateInfoEXT createDebugMsgInfo = {};
	createDebugMsgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createDebugMsgInfo.messageSeverity = this->pVulkanValidationDesiredMsgSeverity->State;
	createDebugMsgInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createDebugMsgInfo.pfnUserCallback = debugCallback;
	createDebugMsgInfo.pUserData = nullptr;

	createInfo.pNext = this->validationLayers ?
		(VkDebugUtilsMessengerCreateInfoEXT*)&createDebugMsgInfo : nullptr;

	// ---- End CreateInfo ----

	// ---- Begin CreateInstance ----
	if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS)
	{
		throw std::runtime_error("vkCreateInstance(...) did not return success");
	}

	// ---- End CreateInstance ----
}

// Validation Layer code
bool VulkanRenderer::validationLayerSupport()
{
	uint32_t numLayers = 0;
	vkEnumerateInstanceLayerProperties(&numLayers, nullptr);

	std::vector<VkLayerProperties> availableLayers(numLayers);
	vkEnumerateInstanceLayerProperties(&numLayers, availableLayers.data());

	// TODO - could continue to run, and just report that we won't be using v layers.
	if (!availableLayers.size())
		throw std::runtime_error("There are no avaliable validation layers"); 

	for (const VkLayerProperties& layerProperties : availableLayers)
	{
		if (strcmp(this->validationLayer, layerProperties.layerName) == 0)
			return true;
	}
	return false;
}

void VulkanRenderer::createSurface()
{
	if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create surface");
	}

}
