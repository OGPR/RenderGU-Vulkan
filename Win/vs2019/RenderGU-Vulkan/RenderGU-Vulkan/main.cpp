#include <iostream>
#include "VulkanWindow.h"
#include "RenderGU_Vk_Renderer.h"
#include <stdexcept>
VulkanWindow vulkanWindow;

bool clArgsRead(int argc,
                char** argv,
                VulkanValidationDesiredMsgSeverity& vulkanValidationDesiredMsgSeverity)
{
    if (argc == 1)
    {
        // no extra args passed - Default behaviour
        // Vulkan Spec states message severity must not be zero, so let's default to error only 
        vulkanValidationDesiredMsgSeverity.State = vulkanValidationDesiredMsgSeverity.ERROR;
        return true;
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
                return false;
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
				return false ;
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
        return true;
    }
    return false; // argc should never be < 1 but we put this here anyway
}

void DrawFrame(const RenderGU_Vk_Renderer& Renderer)
{
    // Acquire an image from renderer swapchain
    uint32_t SwapchainImageIndex;
    vkAcquireNextImageKHR(Renderer.mainDevice.logicalDevice, Renderer.swapchain, UINT64_MAX, Renderer.ImageAvailableSemaphore, VK_NULL_HANDLE, &SwapchainImageIndex);
    
    // Execute the renderer command buffer with image from 1. as attachment in the framebuffer
    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //// Wait for Image to be available before trying to write colors to it
    VkSemaphore WaitSemaphoreArray[] = {Renderer.ImageAvailableSemaphore};
    VkPipelineStageFlags WaitPipelineStageArray[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = WaitSemaphoreArray;
    SubmitInfo.pWaitDstStageMask = WaitPipelineStageArray;

    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &Renderer.CommandBufferContainer[SwapchainImageIndex];

    VkSemaphore SignalSemaphoreArray[] = {Renderer.RenderFinishedSemaphore};
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = SignalSemaphoreArray;

    if (vkQueueSubmit(Renderer.graphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");
    
    // Return the image to the to renderer swapchain for presentation
    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = SignalSemaphoreArray;   //TODO Not WaitSemaphoreArray?

    VkSwapchainKHR SwapChainArray[] = {Renderer.swapchain};
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = SwapChainArray;
    PresentInfo.pImageIndices = &SwapchainImageIndex;

    vkQueuePresentKHR(Renderer.presentationQueue, &PresentInfo);

    vkQueueWaitIdle(Renderer.presentationQueue);
}

int main(int argc, char** argv)
{
	VulkanValidationDesiredMsgSeverity vulkanValidationDesiredMsgSeverity;
    if (!clArgsRead(argc, argv, vulkanValidationDesiredMsgSeverity))
        return 1;

    GLFWwindow* window = vulkanWindow.init("RenderGU_Windows", 800, 600);

	RenderGU_Vk_Renderer _RenderGU_Vk_Renderer(&vulkanValidationDesiredMsgSeverity);
    try
    {
		_RenderGU_Vk_Renderer.Init(window);
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
        DrawFrame(_RenderGU_Vk_Renderer);
    }

    vkDeviceWaitIdle(_RenderGU_Vk_Renderer.mainDevice.logicalDevice);

    _RenderGU_Vk_Renderer.Cleanup();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

}
