#include <iostream>
#include "VulkanWindow.h"
#include "VulkanRenderer.h"
#include <stdexcept>
VulkanWindow vulkanWindow;
VulkanRenderer vulkanRenderer;
int main(int argc, char** argv)
{
    //---START CL ARGS ---//
    if (argc == 1)
    {
        // no extra args passed - Default behaviour
    }
    else if (argc > 1)
    {
        // We have some work to do!
        for (short unsigned i = 2; i <= argc; ++i)
        {
            char* NextArgStr = *(argv + (i - 1));

			if (*NextArgStr != '-')
				break;
			if (*(NextArgStr + 1) == '-')
			{
				std::cout << "Long form for args not supported" << std::endl;
				break;
			}
			// Okay, dealt with initial format, now matching chars
			if (*(NextArgStr + 1) == 'h')
			{
				std::cout << "Help requested!" << std::endl;
				// We don't continue to run, help asked for!
				return 0;
			}

        }
    }
    
    //---END CL ARGS ---//
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
