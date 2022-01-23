#include <iostream>
#include "VulkanWindow.h"
#include "VulkanRenderer.h"
#include <stdexcept>
VulkanWindow vulkanWindow;
VulkanRenderer vulkanRenderer;
int main()
{
    GLFWwindow* window = vulkanWindow.init("RenderGU_Windows", 800, 600);

    try
    {
		vulkanRenderer.init(window);
    }
    catch (const std::exception& e)
    {
        std::cout << "std::exception caught. Message:" << std::endl;
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    


    
    if (!window)
    {
        std::cout << "Window is nullptr, exiting" << std::endl;
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    vulkanRenderer.cleanup();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

}
