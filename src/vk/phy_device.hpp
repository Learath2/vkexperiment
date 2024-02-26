#ifndef INCLUDE_VK_PHY_DEVICE_HPP_
#define INCLUDE_VK_PHY_DEVICE_HPP_

#include <optional>
#include <algorithm>
#include <cstring>
#include <vulkan/vulkan_core.h>

#include "util.hpp"

namespace vkexperiment::vk {

struct SPhyDevice {
	VkPhysicalDevice m_Device;
	VkPhysicalDeviceProperties m_Properties;
	VkPhysicalDeviceFeatures m_Features;
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	std::vector<VkExtensionProperties> m_AvailableExtensions;

	std::optional<uint32_t> m_GraphicsQueueFamily;
	std::optional<uint32_t> m_PresentQueueFamily;
	std::optional<VkSurfaceCapabilitiesKHR> m_SurfaceCapabilities;
	std::optional<std::vector<VkSurfaceFormatKHR>> m_SurfaceFormats;
	std::optional<std::vector<VkPresentModeKHR>> m_SurfacePresentModes;

	// Populated by CCore, bad design
	std::optional<int> m_Score;

	explicit SPhyDevice(VkPhysicalDevice d) : m_Device(d)
	{
		vkGetPhysicalDeviceProperties(m_Device, &m_Properties);
		vkGetPhysicalDeviceFeatures(m_Device, &m_Features);
		m_QueueFamilyProperties = UTIL_VK_ENUMERATE_HELPER([this](auto c, auto d){ vkGetPhysicalDeviceQueueFamilyProperties(m_Device, c, d); return VK_SUCCESS; }, VkQueueFamilyProperties);
		m_AvailableExtensions = UTIL_VK_ENUMERATE_HELPER([this](auto c, auto d){ return vkEnumerateDeviceExtensionProperties(m_Device, nullptr, c, d); }, VkExtensionProperties);
	}

	void ForSurface(const VkSurfaceKHR &Surface)
	{
		// Pick queue families
		decltype(m_QueueFamilyProperties)::size_type i = 0;
		for(const auto &qfp : m_QueueFamilyProperties) {
			if(qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				m_GraphicsQueueFamily = i;

			VkBool32 PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_Device, i, Surface, &PresentSupport);
			if(PresentSupport)
				m_PresentQueueFamily = i;

			if(m_PresentQueueFamily.has_value() && m_PresentQueueFamily.has_value())
				break;

			i++;
		}

		// Get surface capabilities
		VkSurfaceCapabilitiesKHR Capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device, Surface, &Capabilities);
		m_SurfaceCapabilities = std::move(Capabilities);
		
		m_SurfaceFormats = UTIL_VK_ENUMERATE_HELPER(([this, Surface](auto c, auto d){ return vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device, Surface, c, d); }), VkSurfaceFormatKHR);
		m_SurfacePresentModes = UTIL_VK_ENUMERATE_HELPER(([this, Surface](auto c, auto d){ return vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device, Surface, c, d); }), VkPresentModeKHR);
	}

	bool IsExtensionSupported(const char *Wanted)
	{
		return std::ranges::find_if(m_AvailableExtensions, [Wanted](const auto &e){ return !std::strcmp(e.extensionName, Wanted); }) != m_AvailableExtensions.end();
	}
};

}
#endif  // INCLUDE_VK_PHY_DEVICE_HPP_
