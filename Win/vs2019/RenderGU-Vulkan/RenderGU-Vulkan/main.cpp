#define GLFW_INCLUDE_VULKAN // This includes vulkan 
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <assert.h>

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // NO_API so we set up glfw window for Vulkan
    auto window = glfwCreateWindow(800, 600, "RenderGU-Vulkan", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    assert(extensionCount > 0);

    std::cout << "Extension count: " << extensionCount << std::endl;

    // Test GLM
    glm::mat4 IMat(1.0f);
    glm::vec4 Vec(1.0f);
    auto result = IMat * Vec;
    // End Test GLM
        

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

}
