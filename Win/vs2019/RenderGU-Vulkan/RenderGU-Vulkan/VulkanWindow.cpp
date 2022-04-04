#include "VulkanWindow.h"

GLFWwindow* VulkanWindow::init(const char* windowName, int windowWidth, int windowHeight)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // NO_API so we set up glfw window for Vulkan
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
}
