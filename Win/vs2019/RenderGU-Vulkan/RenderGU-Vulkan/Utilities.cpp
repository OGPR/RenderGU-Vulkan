#include "Utilities.h"

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