#ifndef INCLUDE_VK_PIPELINE_HPP_
#define INCLUDE_VK_PIPELINE_HPP_

#include <vulkan/vulkan_core.h>

#include "shader.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"

namespace vkexperiment::vk {

class CPipeline
{
	const CDevice &m_Device;

	CShader m_VertexShader;
	CShader m_FragmentShader;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_Pipeline;

public:
	CPipeline(CShader &&Vertex, CShader &&Fragment, const CDevice &Device, const CSwapChain &SwapChain, const CRenderPass &RenderPass);
	~CPipeline();

	const VkPipeline &Pipeline() const { return m_Pipeline; }
};

}

#endif  // INCLUDE_VK_PIPELINE_HPP_
