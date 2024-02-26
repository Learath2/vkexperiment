#ifndef INCLUDE_VK_COMMANDBUFFER_HPP_
#define INCLUDE_VK_COMMANDBUFFER_HPP_

#include <vulkan/vulkan_core.h>
#include "device.hpp"
#include "renderpass.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

namespace vkexperiment::vk {

class CCommandBuffer
{
	VkCommandBuffer m_CommandBuffer;

public:
	CCommandBuffer(const CDevice &Device, const VkCommandPool &Pool);

	const VkCommandBuffer &CommandBuffer() const { return m_CommandBuffer; }

	void Reset();
	void Record(uint32_t ImageIndex, const CRenderPass &RenderPass, const CPipeline &Pipeline, const CSwapChain &SwapChain);
};

}

#endif  // INCLUDE_VK_COMMANDBUFFER_HPP_
