#include <iostream>
#include <algorithm>

#include "swapchain.hpp"

namespace vkexperiment::vk {

CSwapChain::CSwapChain(CDevice &d, CWindow &w, VkSurfaceKHR &s) : m_Device(d), m_Window(w)
{
	auto &PhyDevice = m_Device.PhyDevice();

	auto SurfaceFormat = PickSurfaceFormat(PhyDevice.m_SurfaceFormats.value());
	auto PresentMode = PickPresentMode(PhyDevice.m_SurfacePresentModes.value());
	auto SwapExtent = PickSwapExtent(PhyDevice.m_SurfaceCapabilities.value());

	uint32_t ImageCount = PhyDevice.m_SurfaceCapabilities->minImageCount + 1;
	if(PhyDevice.m_SurfaceCapabilities->maxImageCount != 0 && ImageCount > PhyDevice.m_SurfaceCapabilities->maxImageCount)
		ImageCount = PhyDevice.m_SurfaceCapabilities->maxImageCount;

	std::cout<<"Creating swapchain with "<<ImageCount<<" images"<<std::endl;

	VkSwapchainCreateInfoKHR CreateInfo{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = s,
		.minImageCount = ImageCount,
		.imageFormat = SurfaceFormat.format,
		.imageColorSpace = SurfaceFormat.colorSpace,
		.imageExtent = SwapExtent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = PhyDevice.m_SurfaceCapabilities->currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = PresentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
	};

	uint32_t QueueFamilyIndices[] = {PhyDevice.m_GraphicsQueueFamily.value(), PhyDevice.m_GraphicsQueueFamily.value()};
	if(PhyDevice.m_GraphicsQueueFamily != PhyDevice.m_PresentQueueFamily) {
		CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		CreateInfo.queueFamilyIndexCount = 2;
		CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
	}
	else {
		CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0;
		CreateInfo.pQueueFamilyIndices = nullptr;
	}

	if(vkCreateSwapchainKHR(m_Device.Device(), &CreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
		throw std::runtime_error{"Failed to create swapchain"};

	// Get the images
	m_Images = UTIL_VK_ENUMERATE_HELPER([this](auto c, auto d){ return vkGetSwapchainImagesKHR(m_Device.Device(), m_Handle, c, d); }, VkImage);
	m_ImageFormat = SurfaceFormat.format;
	m_Extent = SwapExtent;

	// Get the image views
	m_ImageViews.resize(m_Images.size());
	for(decltype(m_Images)::size_type i = 0; i < m_Images.size(); i++) {
		VkImageViewCreateInfo CreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_Images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_ImageFormat,
			.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
			.subresourceRange = VkImageSubresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
		};

		if(vkCreateImageView(m_Device.Device(), &CreateInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error{"Couldn't create swapchain image views"};
	}
}

CSwapChain::~CSwapChain()
{
	for(auto v : m_ImageViews)
		vkDestroyImageView(m_Device.Device(), v, nullptr);

	vkDestroySwapchainKHR(m_Device.Device(), m_Handle, nullptr);
}

VkSurfaceFormatKHR CSwapChain::PickSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &AvailableFormats)
{
	for(const auto &f : AvailableFormats)
		if(f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return f;

	return AvailableFormats[0];
}

VkPresentModeKHR CSwapChain::PickPresentMode(const std::vector<VkPresentModeKHR> &AvailablePresentModes)
{
	for(const auto &pm : AvailablePresentModes)
		if(pm == VK_PRESENT_MODE_MAILBOX_KHR)
			return pm;

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CSwapChain::PickSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities)
{
	if(Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return Capabilities.currentExtent;
	else {
		auto [W, H] = m_Window.FramebufferSize();
		return VkExtent2D{std::clamp(static_cast<uint32_t>(W), Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(H), Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height)};
	}
}

}
