#include <iostream>
#include "VulkanWindow.h"

VulkanWindow vulkanWindow;
int main()
{
    GLFWwindow* window = vulkanWindow.init("RenderGU_Windows", 800, 600);
    
    if (!window)
    {
        std::cout << "Window is nullptr, exiting" << std::endl;
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

}
