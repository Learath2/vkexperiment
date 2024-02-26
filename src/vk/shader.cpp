#include <fstream>
#include <memory>
#include <cstdint>
#include <stdexcept>

#include <base/util.hpp>

#include "shader.hpp"

namespace vkexperiment::vk {
CShader::CShader(CDevice &Device, const char *Filepath) : m_Device(Device)
{
	auto [buf, Size] = base::util::readfile(Filepath);

	VkShaderModuleCreateInfo CreateInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = Size,
		.pCode = reinterpret_cast<uint32_t *>(buf.get()),
	};

	if(vkCreateShaderModule(Device.Device(), &CreateInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create VkShaderModule"};
}

CShader::~CShader()
{
	vkDestroyShaderModule(m_Device.Device(), m_ShaderModule, nullptr);
}
}
