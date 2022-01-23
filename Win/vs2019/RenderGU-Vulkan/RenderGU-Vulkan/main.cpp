#include <iostream>
#include "VulkanWindow.h"

VulkanWindow vulkanWindow;
int main()
{
    GLFWwindow* window = vulkanWindow.init("RenderGU_Windows", 800, 600);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

}
