#ifndef INCLUDE_VK_CORE_HPP_
#define INCLUDE_VK_CORE_HPP_

#include <string>
#include <vulkan/vulkan_core.h>

#include "window.hpp"
#include "phy_device.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "pipeline.hpp"
#include "commandbuffer.hpp"

#ifndef NDEBUG
#define IS_DEBUG true
#else
#define IS_DEBUG false
#endif // NDEBUG

namespace vkexperiment::vk {
class CCore {
	static constexpr bool ms_EnableValidationLayers = IS_DEBUG; 

	VkInstance m_Instance;
	VkSurfaceKHR m_Surface;
	std::optional<SPhyDevice> m_PickedPhyDevice;
	std::optional<CDevice> m_Device;
	std::optional<CSwapChain> m_Swapchain;
	std::optional<CRenderPass> m_RenderPass;
	std::optional<CPipeline> m_Pipeline;
	std::optional<VkCommandPool> m_CommandPool;
	std::optional<CCommandBuffer> m_CommandBuffer;
	
	VkSemaphore m_ImageAvailable;
	VkSemaphore m_RenderFinished;
	VkFence m_InFlight;

	void CheckLayers(const std::vector<const char *> &RequiredLayers);
	void CreateInstance(std::string AppName, uint32_t AppVersion, std::vector<const char *> &RequiredLayers, std::vector<const char *> &RequiredInstanceExtensions);
	void CreateSurface(CWindow &Window);
	int DeviceScore(const SPhyDevice &Device);
	void PickPhysicalDevice(std::vector<const char *> &RequiredDeviceExtensions);
	void CreateLogicalDevice(std::vector<const char *> &RequiredLayers, std::vector<const char *> &RequiredDeviceExtensions);
	void CreateSwapChain(CWindow &Window);
	void CreateRenderPass();
	void CreatePipeline(std::string VertexShaderPath, std::string FragmentShaderPath);
	void CreateFramebuffers();
	void CreateCommandBuffer();
	void CreateSyncObjects();

public:
	explicit CCore(std::string AppName, uint32_t AppVersion, CWindow &Window, std::vector<const char *> &&RequiredLayers = {}, std::vector<const char *> &&RequiredInstanceExtensions = {}, std::vector<const char *> &&RequiredDeviceExtensions = {});
	~CCore();

	void WaitIdle();
	void Draw();
};
}

#endif  // INCLUDE_VK_CORE_HPP_

