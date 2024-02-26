#ifndef INCLUDE_VK_DEVICE_HPP_
#define INCLUDE_VK_DEVICE_HPP_

#include <vulkan/vulkan_core.h>

#include "phy_device.hpp"

namespace vkexperiment::vk {

class CDevice {
	SPhyDevice m_PhyDevice;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

public:
	explicit CDevice(VkDevice &&d, SPhyDevice &p) : m_Device(d), m_PhyDevice(p) {
		vkGetDeviceQueue(m_Device, p.m_GraphicsQueueFamily.value(), 0, &m_GraphicsQueue); // Not a massive fan of the hardcoded 0, will think about it
		vkGetDeviceQueue(m_Device, p.m_PresentQueueFamily.value(), 0, &m_PresentQueue);
	};
	~CDevice() { vkDestroyDevice(m_Device, nullptr); }

	

	const VkDevice &Device() const { return m_Device; }
	const SPhyDevice &PhyDevice() const { return m_PhyDevice; }
	const VkQueue &GraphicsQueue() const { return m_GraphicsQueue; }
	const VkQueue &PresentQueue() const { return m_PresentQueue; }

};

}

#endif  // INCLUDE_VK_DEVICE_HPP_
