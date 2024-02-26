#include "commandbuffer.hpp"

namespace vkexperiment::vk {

CCommandBuffer::CCommandBuffer(const CDevice &Device, const VkCommandPool &Pool)
{
	VkCommandBufferAllocateInfo AllocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = Pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	if(vkAllocateCommandBuffers(Device.Device(), &AllocInfo, &m_CommandBuffer) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't allocate command buffer"};
}

void CCommandBuffer::Reset()
{
	vkResetCommandBuffer(m_CommandBuffer, 0);
}

void CCommandBuffer::Record(uint32_t ImageIndex, const CRenderPass &RenderPass, const CPipeline &Pipeline, const CSwapChain &SwapChain)
{
	VkCommandBufferBeginInfo BeginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = nullptr,
	};

	if(vkBeginCommandBuffer(m_CommandBuffer, &BeginInfo) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't begin recording command buffer"};

	VkClearValue ClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // black
	VkRenderPassBeginInfo RenderPassBeginInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = RenderPass.RenderPass(),
		.framebuffer = RenderPass.Framebuffers()[ImageIndex],
		.renderArea = {.offset = {0, 0}, .extent = SwapChain.Extent()},
		.clearValueCount = 1,
		.pClearValues = &ClearColor,
	};

	vkCmdBeginRenderPass(m_CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline.Pipeline());

	VkViewport Viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(SwapChain.Extent().width),
		.height = static_cast<float>(SwapChain.Extent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	vkCmdSetViewport(m_CommandBuffer, 0, 1, &Viewport);

	VkRect2D Scissor{
		.offset = {0, 0},
		.extent = SwapChain.Extent(),
	};
	vkCmdSetScissor(m_CommandBuffer, 0, 1, &Scissor);

	vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(m_CommandBuffer);

	if(vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		throw std::runtime_error{"Failed to record command buffer"};
}

}
