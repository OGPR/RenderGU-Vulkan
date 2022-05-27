#include "Utilities.h"
#include <assert.h> 

namespace RenderGU_Vk_Utils
{
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

	std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageInfoContainer(
		std::vector<std::tuple<VkShaderModule, VkShaderStageFlagBits, const char*>>& ShaderInfoContainer)
	{
		std::vector<VkPipelineShaderStageCreateInfo> OutputContainer;
		for (auto& ShaderInfo : ShaderInfoContainer)
		{
			VkPipelineShaderStageCreateInfo StageInfo = {};
			StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			StageInfo.stage = std::get<1>(ShaderInfo);
			StageInfo.module = std::get<0>(ShaderInfo);
			StageInfo.pName = std::get<2>(ShaderInfo);

			OutputContainer.push_back(StageInfo);
		}

		return OutputContainer;
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(
		VkShaderModule ShaderModule,
		VkShaderStageFlagBits ShaderStageFlagBits,
		const char* ShaderEntryPointName
	)
	{
		VkPipelineShaderStageCreateInfo ShaderStageInfo = {};
		ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageInfo.stage = ShaderStageFlagBits;
		ShaderStageInfo.module = ShaderModule;
		ShaderStageInfo.pName = ShaderEntryPointName;

		return ShaderStageInfo;
	}
};