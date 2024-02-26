#ifndef INCLUDE_VK_RENDERPASS_HPP_
#define INCLUDE_VK_RENDERPASS_HPP_

#include <vector>
#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "swapchain.hpp"

namespace vkexperiment::vk {

class CRenderPass {
	const CDevice &m_Device;

	VkRenderPass m_RenderPass;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

public:
	CRenderPass(const CDevice &Device, const CSwapChain &Swapchain);
	~CRenderPass();

	void CreateFramebuffers(const CSwapChain &Swapchain);

	const VkRenderPass &RenderPass() const { return m_RenderPass; }
	const auto &Framebuffers() const { return m_SwapChainFramebuffers; }
};

}

#endif  // INCLUDE_VK_RENDERPASS_HPP_
