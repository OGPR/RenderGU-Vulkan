#include "Utilities.h"
#include <assert.h> 

RenderGU_BytecodeBuffer ReadBytecode(const std::string Filename)
{
	std::fstream Filestream(Filename, std::ios::in | std::ios::ate | std::ios::binary);

	if (!Filestream.is_open())
		throw std::runtime_error("Failed to open " + Filename);

	RenderGU_BytecodeBuffer BytecodeBuffer(Filestream.tellg());
	Filestream.seekg(0);
	Filestream.read(BytecodeBuffer.data(), std::size(BytecodeBuffer));
	Filestream.close();

	return BytecodeBuffer;
}

VkShaderModule CreateShaderModule(VkDevice LogicalDevice,
	RenderGU_BytecodeBuffer& BytecodeBuffer)
{
	if (!LogicalDevice)
		throw std::runtime_error("RenderGU_Vk_Renderer: No logical device");

	if (!std::size(BytecodeBuffer))
		throw std::runtime_error("RenderGU_Vk_Renderer: Bytecode buffer is size zero");

	VkShaderModule ShaderModule = nullptr;

	VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = std::size(BytecodeBuffer);
	ShaderModuleCreateInfo.pCode = (const uint32_t*)BytecodeBuffer.data();


	VkResult CreateShaderModuleResult = vkCreateShaderModule(
		LogicalDevice,
		&ShaderModuleCreateInfo,
		nullptr,
		&ShaderModule
	);

	if (CreateShaderModuleResult != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex shader module");

	assert(ShaderModule);
	return ShaderModule;
}