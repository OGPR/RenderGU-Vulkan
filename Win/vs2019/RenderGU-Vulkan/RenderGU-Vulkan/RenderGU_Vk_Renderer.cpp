#include "RenderGU_Vk_Renderer.h"
#include <iostream>
#include <set>
#include <assert.h>

int RenderGU_Vk_Renderer::init(GLFWwindow* window)
{
	this->window = window;

	createInstance();
	setupDebugMessenger();
	createSurface();
	getPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();


	return 0;
}

void RenderGU_Vk_Renderer::setupDebugMessenger()
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

void RenderGU_Vk_Renderer::destroyDebugMessenger()
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

VKAPI_ATTR VkBool32 VKAPI_CALL RenderGU_Vk_Renderer::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "Custom callback : Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
	//return VK_TRUE;
}

void RenderGU_Vk_Renderer::cleanup()
{
	if (this->validationLayers)
	{
		destroyDebugMessenger();
	}
	vkDestroySwapchainKHR(this->mainDevice.logicalDevice, this->swapchain, nullptr);
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

	uint32_t count = 0;
	while (count < this->SwapChainImageCount)
		vkDestroyImageView(mainDevice.logicalDevice, ImageViewArray[count++], nullptr);

	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(this->instance, nullptr);

}

bool RenderGU_Vk_Renderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
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

bool RenderGU_Vk_Renderer::checkPhysicalDeviceExtensionSupport(
	VkPhysicalDevice device,
	const std::vector<const char*>& desiredPhysicalDeviceExtensions)
{
	uint32_t physicalDeviceExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &physicalDeviceExtensionCount, nullptr);

	if (!physicalDeviceExtensionCount)
		return false;

	std::vector<VkExtensionProperties> physicalDeviceExtensionList(physicalDeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &physicalDeviceExtensionCount, physicalDeviceExtensionList.data());

	for (const char* desiredPhysicalDeviceExtension : desiredPhyiscalDeviceExtenstions)
	{
		bool desiredPhysicalDeviceExtensionFound = false;
		for (const VkExtensionProperties& physicalDeviceExtension : physicalDeviceExtensionList)
		{
			if (strcmp(physicalDeviceExtension.extensionName, desiredPhysicalDeviceExtension) == 0)
			{
				desiredPhysicalDeviceExtensionFound = true;
				break;
			}
		}

		if (!desiredPhysicalDeviceExtensionFound)
			return false;

	}

	return true;
}

bool RenderGU_Vk_Renderer::checkDeviceSuitable(VkPhysicalDevice device)
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
	
	bool desiredPhysicalDeviceExtensionsSupported =
		checkPhysicalDeviceExtensionSupport(
			device,
			desiredPhyiscalDeviceExtenstions
		);

	bool swapChainDescValid = CreateSwapChainDesc(device).isValid();

	return swapChainDescValid && desiredPhysicalDeviceExtensionsSupported && indices.isValid();
}

QueueFamilyIndices RenderGU_Vk_Renderer::getQueueFamilyIndices(VkPhysicalDevice device)
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

void RenderGU_Vk_Renderer::getPhysicalDevice()
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

void RenderGU_Vk_Renderer::createLogicalDevice()
{
	// Specify the queues the logical device needs to create
	QueueFamilyIndices queueIndicies = getQueueFamilyIndices(mainDevice.physicalDevice);
	if (!queueIndicies.isValid())
		throw std::runtime_error("Queue index not valid!");

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
	std::set<int> queueFamiliyIndexSet
	{
		queueIndicies.graphicsFamily,
		queueIndicies.presentationFamily
	};

	for (int i : queueFamiliyIndexSet)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueIndicies.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		float priority = 1.0f;
		queueCreateInfo.pQueuePriorities = &priority;

		queueCreateInfoList.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};


	VkDeviceCreateInfo logicalDeviceCreateInfo = {};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(std::size(queueCreateInfoList));
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(std::size(desiredPhyiscalDeviceExtenstions));
	logicalDeviceCreateInfo.ppEnabledExtensionNames = desiredPhyiscalDeviceExtenstions.data();
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
	vkGetDeviceQueue(mainDevice.logicalDevice, queueIndicies.graphicsFamily, 0, &presentationQueue);
}

void RenderGU_Vk_Renderer::createInstance()
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
bool RenderGU_Vk_Renderer::validationLayerSupport()
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

void RenderGU_Vk_Renderer::createSurface()
{
	if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create surface");
	}

}

SwapChainDesc RenderGU_Vk_Renderer::CreateSwapChainDesc(VkPhysicalDevice physicalDevice)
{
	SwapChainDesc swapChainDesc = {};

	// Surface Capabilities
	assert(this->surface);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, this->surface, &swapChainDesc.surfaceCapabilities);

	// Surface Formats
	assert(this->surface);
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, this->surface, &surfaceFormatCount, nullptr);

	assert(surfaceFormatCount);
	swapChainDesc.surfaceFormatArray.resize(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, this->surface, &surfaceFormatCount, swapChainDesc.surfaceFormatArray.data());

	// Presentation modes
	assert(this->surface);
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, this->surface, &presentModeCount, nullptr);

	assert(presentModeCount);
	swapChainDesc.presentationModeArray.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, this->surface, &presentModeCount, swapChainDesc.presentationModeArray.data());

	return swapChainDesc;

}

void RenderGU_Vk_Renderer::createSwapChain()
{
	SwapChainDesc swapChainDesc = CreateSwapChainDesc(mainDevice.physicalDevice);

	// Check if our desired surface format is available in SwapChainDesc
	assert(swapChainDesc.surfaceFormatArray.size());
	bool foundDesiredSurfaceFormat = false;
	bool foundDesiredSurface = false;
	for (VkSurfaceFormatKHR surfaceFormat : swapChainDesc.surfaceFormatArray)
	{
		if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
			throw std::runtime_error("Surface format undefined!");
			// TODO check with spec - if undefined it might mean all formats are available

		if (surfaceFormat.format == DesiredSurfaceFormat::Format &&
			surfaceFormat.colorSpace == DesiredSurfaceFormat::ColorSpace)
		{
			foundDesiredSurfaceFormat = true;
			break;
		}
	}

	if (!foundDesiredSurfaceFormat)
		throw std::runtime_error("Cannot find our desired surface format from the swap chain desc");

	

	// Check if our desired present mode is available in SwapChainDesc
	assert(swapChainDesc.presentationModeArray.size());
	bool foundDesiredPresentMode = false;
	for (VkPresentModeKHR presentMode : swapChainDesc.presentationModeArray)
	{
		if (presentMode == DesiredPresentation::Mode)
		{
			foundDesiredPresentMode = true;
			break;
		}
	}
	
	if(!foundDesiredPresentMode)
		throw std::runtime_error("Cannot find our desired present mode from the swap chain desc");
		//TODO check with spec - we should be able to get FIFO

	// Swap chain image resolution
	if (swapChainDesc.surfaceCapabilities.currentExtent.width == UINT_FAST32_MAX)
		throw std::runtime_error("Extent is changing");
	//TODO handle this. Also use platform independent UINT max

	// This is where we create the swapchain
	assert(this->mainDevice.logicalDevice);

	// We are going to assume this. We can then make the image sharing mode in the
	// swapchaincreateinfo exclusive.
	// TODO this check and call - perhaps we could store the retrieved queue
	// family indices at our renderer class scope
	// TODO handle the case where the indices are different (use concurrent mode)
	assert(getQueueFamilyIndices(mainDevice.physicalDevice).graphicsFamily ==
		getQueueFamilyIndices(mainDevice.physicalDevice).presentationFamily);

	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.queueFamilyIndexCount = 0;
	swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = this->surface;
	swapChainCreateInfo.preTransform = swapChainDesc.surfaceCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageExtent = swapChainDesc.surfaceCapabilities.currentExtent;
	swapChainCreateInfo.minImageCount = swapChainDesc.surfaceCapabilities.minImageCount;
	swapChainCreateInfo.imageFormat = DesiredSurfaceFormat::Format;
	swapChainCreateInfo.imageColorSpace = DesiredSurfaceFormat::ColorSpace;
	swapChainCreateInfo.clipped = VK_FALSE; // Yes,  false - I want images to own all their pixels
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	VkResult vkRes = vkCreateSwapchainKHR(this->mainDevice.logicalDevice,
		&swapChainCreateInfo,
		nullptr,
		&this->swapchain
	);

	assert(&this->swapchain);

	
}

void RenderGU_Vk_Renderer::createImageViews()
{
	assert(this->swapchain);
	uint32_t swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice,
		swapchain,
		&swapChainImageCount,
		nullptr);

	assert(swapChainImageCount);
	std::vector<VkImage> swapChainImageArray(swapChainImageCount);
	this->SwapChainImageCount = swapChainImageCount;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice,
		swapchain,
		&swapChainImageCount,
		swapChainImageArray.data());

	assert(swapChainImageArray.size());
	ImageViewArray.resize(swapChainImageCount);

	for (int i = 0; i < swapChainImageArray.size(); ++i)
	{
		VkImageViewCreateInfo ImageViewCreateInfo = {};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.image = swapChainImageArray[i];
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format = DesiredSurfaceFormat::Format;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		

		VkResult vkRes = vkCreateImageView(
			mainDevice.logicalDevice,
			&ImageViewCreateInfo,
			nullptr,
			&ImageViewArray[i]
		);

		assert(ImageViewArray[i]);
	}









}
