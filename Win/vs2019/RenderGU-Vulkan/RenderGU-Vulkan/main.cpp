#include <iostream>
#include "VulkanWindow.h"
#include "VulkanRenderer.h"
#include <stdexcept>
VulkanWindow vulkanWindow;
VulkanValidationDesiredMsgSeverity vulkanValidationDesiredMsgSeverity;

int main(int argc, char** argv)
{
    //---START CL ARGS ---//
    if (argc == 1)
    {
        // no extra args passed - Default behaviour
        // Vulkan Spec states message severity must not be zero, so let's default to error only 
        vulkanValidationDesiredMsgSeverity.State = vulkanValidationDesiredMsgSeverity.ERROR;
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
            if (*(NextArgStr + 1) == '\0')
            {
                std::cout << "Require a cl option after -. See -h for more info" << std::endl;
                return 1;
            }
			// Okay, dealt with initial format, now matching chars
			if (*(NextArgStr + 1) == 'h')
			{
				std::cout << "Usage: " << std::endl;
                std::cout << " -<...>" << std::endl;
                std::cout << "where <...> contains any of:" << std::endl;
                std::cout << "d : Diagnostic message (corresponding to VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT" << std::endl;
                std::cout << "i : Informational message (corresponding to VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT" << std::endl;
                std::cout << "w : Warning message (corresponding to VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT" << std::endl;
                std::cout << "e : Error message (corresponding to VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT" << std::endl;
                std::cout << "" << std::endl;
                std::cout << "Default behaviour is -e" << std::endl;
				// We don't continue to run, help asked for!
				return 0;
			}

            //Work through argument string
            char* CurrArgStr  = NextArgStr + 1;
            while (*CurrArgStr != '\0')
            {
                if (*CurrArgStr == 'd')
                {
                    vulkanValidationDesiredMsgSeverity.State
                        |= vulkanValidationDesiredMsgSeverity.DIAG;

                }
                if (*CurrArgStr == 'i')
                {
                    vulkanValidationDesiredMsgSeverity.State
                        |= vulkanValidationDesiredMsgSeverity.INFO;

                }
                if (*CurrArgStr == 'w')
                {
                    vulkanValidationDesiredMsgSeverity.State
                        |= vulkanValidationDesiredMsgSeverity.WARN;
                }
                if (*CurrArgStr == 'e')
                {
                    vulkanValidationDesiredMsgSeverity.State
                        |= vulkanValidationDesiredMsgSeverity.ERROR;
                }
				++CurrArgStr;
            }


        }
        // For Intial CL args testing (stop rest of program)
        // TODO remove when ready
    }
    
    //---END CL ARGS ---//
    GLFWwindow* window = vulkanWindow.init("RenderGU_Windows", 800, 600);

	VulkanRenderer vulkanRenderer(&vulkanValidationDesiredMsgSeverity);
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
