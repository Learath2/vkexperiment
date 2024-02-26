#ifndef INCLUDE_VK_SWAPCHAIN_HPP_
#define INCLUDE_VK_SWAPCHAIN_HPP_

#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "window.hpp"

namespace vkexperiment::vk {

class CSwapChain {
	VkSwapchainKHR m_Handle;
	CDevice &m_Device;
	CWindow &m_Window;

	VkFormat m_ImageFormat;
	VkExtent2D m_Extent;
	std::vector<VkImage> m_Images;
	std::vector<VkImageView> m_ImageViews;

	VkSurfaceFormatKHR PickSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &AvailableFormats);
	VkPresentModeKHR PickPresentMode(const std::vector<VkPresentModeKHR> &AvailablePresentModes);
	VkExtent2D PickSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities);

public:
	CSwapChain(CDevice &d, CWindow &w, VkSurfaceKHR &s);
	~CSwapChain();

	const VkSwapchainKHR &SwapChain() const { return m_Handle; }
	VkExtent2D Extent() const { return m_Extent; }
	VkFormat ImageFormat() const { return m_ImageFormat; }
	const auto &ImageViews() const { return m_ImageViews; }
};

}

#endif  // INCLUDE_VK_SWAPCHAIN_HPP_
