#ifndef INCLUDE_SRC_SHADER_HPP_
#define INCLUDE_SRC_SHADER_HPP_

#include <vulkan/vulkan_core.h>

#include "device.hpp"

namespace vkexperiment::vk {
class CShader {
	CDevice &m_Device;
	VkShaderModule m_ShaderModule;

public:
	CShader(CDevice &Device, const char *Filepath);
	~CShader();

	const VkShaderModule &Module() { return m_ShaderModule; }
};
}

#endif  // INCLUDE_SRC_SHADER_HPP_
