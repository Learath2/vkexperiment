#include "renderpass.hpp"

namespace vkexperiment::vk {

CRenderPass::CRenderPass(const CDevice &Device, const CSwapChain &SwapChain) : m_Device(Device) 
{
	VkAttachmentDescription ColorAttachment{
		.format = SwapChain.ImageFormat(),
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	VkAttachmentReference ColorAttachmentRef{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	VkSubpassDescription Subpass{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &ColorAttachmentRef
	};

	VkSubpassDependency Dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	VkRenderPassCreateInfo CreateInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &ColorAttachment,
		.subpassCount = 1,
		.pSubpasses = &Subpass,
		.dependencyCount = 1,
		.pDependencies = &Dependency,
	};

	if(vkCreateRenderPass(Device.Device(), &CreateInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create render pass"};
}

CRenderPass::~CRenderPass()
{
	for(const auto &fb : m_SwapChainFramebuffers)
		vkDestroyFramebuffer(m_Device.Device(), fb, nullptr);

	vkDestroyRenderPass(m_Device.Device(), m_RenderPass, nullptr);
}

void CRenderPass::CreateFramebuffers(const CSwapChain &SwapChain)
{
	m_SwapChainFramebuffers.resize(SwapChain.ImageViews().size());
	for(int i = 0; i < SwapChain.ImageViews().size(); i++) {
		VkFramebufferCreateInfo CreateInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = m_RenderPass,
			.attachmentCount = 1,
			.pAttachments = &SwapChain.ImageViews()[i],
			.width = SwapChain.Extent().width,
			.height = SwapChain.Extent().height,
			.layers = 1,
		};

		if(vkCreateFramebuffer(m_Device.Device(), &CreateInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error{std::format("Couldn't create framebuffer {}", i)};
	}
}

}
