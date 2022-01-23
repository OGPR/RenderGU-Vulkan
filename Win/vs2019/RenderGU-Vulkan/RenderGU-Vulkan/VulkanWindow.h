#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct VulkanWindow
{
	GLFWwindow* init(const char* windowName, int windowWidth, int windowHeight);
};

