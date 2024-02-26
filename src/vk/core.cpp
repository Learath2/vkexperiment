#include <base/util.hpp>

#include "version.hpp"
#include "core.hpp"
#include "util.hpp"

#include <set>
#include <cstring>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <format>
#include <vector>
#include <stdexcept>
#include <GLFW/glfw3.h>

namespace vkexperiment::vk {

CCore::CCore(std::string AppName, uint32_t AppVersion, CWindow &Window, std::vector<const char *> &&RequiredLayers, std::vector<const char *> &&RequiredInstanceExtensions, std::vector<const char *> &&RequiredDeviceExtensions)
{
	if constexpr(CCore::ms_EnableValidationLayers) 
		RequiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");

	base::util::dedupe(RequiredLayers);

	CheckLayers(RequiredLayers);
	CreateInstance(AppName, AppVersion, RequiredLayers, RequiredInstanceExtensions);
	CreateSurface(Window);
	PickPhysicalDevice(RequiredDeviceExtensions);
	CreateLogicalDevice(RequiredLayers, RequiredDeviceExtensions);
	CreateSwapChain(Window);
	CreateRenderPass();
	CreatePipeline("shaders/vert.spv", "shaders/frag.spv");
	CreateFramebuffers();
	CreateCommandBuffer();
	CreateSyncObjects();
}

CCore::~CCore()
{
	vkDestroyCommandPool(m_Device->Device(), m_CommandPool.value(), nullptr);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
}

void CCore::CheckLayers(const std::vector<const char *> &RequiredLayers)
{
	if(RequiredLayers.empty())
		return;

	bool LayerMissing = false;
	auto AvailableLayers = UTIL_VK_ENUMERATE_HELPER(vkEnumerateInstanceLayerProperties, VkLayerProperties);
	for(const auto &l : RequiredLayers) {
		if(std::ranges::find_if(AvailableLayers, [l](const VkLayerProperties &Props){ return !std::strcmp(l, Props.layerName); }) == AvailableLayers.end()) {
			LayerMissing = true;
			std::cout << "Missing requested layer: " << l << std::endl;
		}
	}

	if(LayerMissing)
		throw std::runtime_error{"Requested layers not found"};
}

void CCore::CreateInstance(std::string AppName, uint32_t AppVersion, std::vector<const char *> &RequiredLayers, std::vector<const char *> &RequiredInstanceExtensions)
{
	uint32_t GLFWRequiredExtensionCount = 0;
	const char **GLFWRequiredExtensions;
	GLFWRequiredExtensions = glfwGetRequiredInstanceExtensions(&GLFWRequiredExtensionCount);
	std::span GLFWRequiredExtensionsSpan{GLFWRequiredExtensions, GLFWRequiredExtensionCount};

	RequiredInstanceExtensions.insert(RequiredInstanceExtensions.end(), GLFWRequiredExtensionsSpan.begin(), GLFWRequiredExtensionsSpan.end());
	base::util::dedupe(RequiredInstanceExtensions);

	std::vector AvailableExtensions = UTIL_VK_ENUMERATE_HELPER([](auto c, auto d){ return vkEnumerateInstanceExtensionProperties(nullptr, c, d); }, VkExtensionProperties);
	std::cout << "Available extensions:" << std::endl;
	for(auto &[ExName, ExVer] : AvailableExtensions) {
		std::cout << std::format("\t{}: {}", ExName, ExVer) << std::endl;
	}

	bool ExtensionMissing = false;
	for(const auto &e : RequiredInstanceExtensions) {
		if(std::ranges::find_if(AvailableExtensions, [e](const VkExtensionProperties &Props){ return !std::strcmp(e, Props.extensionName); }) == AvailableExtensions.end()) {
			ExtensionMissing = true;
			std::cout << "Missing required extension: " << e << std::endl;
		}
	}

	if(ExtensionMissing)
		throw std::runtime_error{"Missing required VK extension(s)"};

	VkApplicationInfo AppInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = AppName.c_str(),
		.applicationVersion = AppVersion,
		.pEngineName = ENGINE_NAME,
		.engineVersion = VERSION_U32,
		.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
	};

	VkInstanceCreateInfo CreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &AppInfo,
		.enabledLayerCount = static_cast<uint32_t>(RequiredLayers.size()),
		.ppEnabledLayerNames = RequiredLayers.size() ? RequiredLayers.data() : nullptr,
		.enabledExtensionCount = static_cast<uint32_t>(RequiredInstanceExtensions.size()),
		.ppEnabledExtensionNames = RequiredInstanceExtensions.size() ? RequiredInstanceExtensions.data() : nullptr,
	};

	if(vkCreateInstance(&CreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create VkInstance"};
}

void CCore::CreateSurface(CWindow &Window)
{
	if(glfwCreateWindowSurface(m_Instance, Window.Window(), nullptr, &m_Surface))
		throw std::runtime_error{"Couldn't create surface"};
}

static void ScoreDevice(SPhyDevice &Device, const std::vector<const char *> &RequiredExtensions = {})
{
	int Score = 0;

	if(!Device.m_Features.geometryShader || !Device.m_GraphicsQueueFamily.has_value() || !Device.m_PresentQueueFamily.has_value())
		goto unsuitable;

	for(const auto &e : RequiredExtensions)
		if(!Device.IsExtensionSupported(e))
			goto unsuitable;

	if(Device.m_SurfaceFormats->empty() || Device.m_SurfacePresentModes->empty())
		goto unsuitable;

	if(Device.m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		Score += 100;

	//Score += Properties.limits.maxImageDimension2D;
	Device.m_Score = Score;
	return;

unsuitable:
	Device.m_Score = -1;
}


void CCore::PickPhysicalDevice(std::vector<const char *> &RequiredDeviceExtensions)
{
	RequiredDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	base::util::dedupe(RequiredDeviceExtensions);

	auto AvailablePhyDevicesRaw = UTIL_VK_ENUMERATE_HELPER([this](auto c, auto d){ return vkEnumeratePhysicalDevices(m_Instance, c, d); }, VkPhysicalDevice);
	if(AvailablePhyDevicesRaw.size() == 0)
		throw std::runtime_error{"Couldn't find a Vulkan capable GPU"};

	std::vector<SPhyDevice> AvailablePhyDevices{AvailablePhyDevicesRaw.begin(), AvailablePhyDevicesRaw.end()};
	for(auto &d : AvailablePhyDevices) {
		d.ForSurface(m_Surface);
		ScoreDevice(d, RequiredDeviceExtensions);
	}

	std::ranges::sort(AvailablePhyDevices, std::ranges::greater(), [](const auto &d){ return d.m_Score.value(); });
	std::cout << "Available physical devices:" << std::endl;
	for(const auto &d : AvailablePhyDevices)
		std::cout << std::format("\t{} : {}", d.m_Properties.deviceName, d.m_Score.value()) << std::endl;
	
	auto SuitablePhyDevices = std::ranges::subrange(AvailablePhyDevices.begin(), std::ranges::find_if(AvailablePhyDevices, [](const auto &d){ return d.m_Score.value() == -1; }));
	std::cout << "Suitable physical devices:" << std::endl;
	for(const auto &d : SuitablePhyDevices)
		std::cout << std::format("\t{} : {}", d.m_Properties.deviceName, d.m_Score.value()) << std::endl;

	if(SuitablePhyDevices.empty())
		throw std::runtime_error{"Couldn't find a suitable device"};

	SPhyDevice &PickedDevice = *SuitablePhyDevices.begin();
	std::cout << "Using device: " << PickedDevice.m_Properties.deviceName << std::endl;
	std::cout << "Using queue: " << PickedDevice.m_GraphicsQueueFamily.value() << std::endl;

	m_PickedPhyDevice = PickedDevice;
}

void CCore::CreateLogicalDevice(std::vector<const char *> &RequiredLayers, std::vector<const char *> &RequiredDeviceExtensions)
{
	auto &PhyDevice = m_PickedPhyDevice.value();

	float Priority = 1.0f;
	std::set<uint32_t> QueueFamilies{PhyDevice.m_GraphicsQueueFamily.value(), PhyDevice.m_PresentQueueFamily.value()};
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	QueueCreateInfos.reserve(QueueFamilies.size());
	for(auto qf : QueueFamilies)
		QueueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = static_cast<uint32_t>(PhyDevice.m_GraphicsQueueFamily.value()), // Always safe since VK guarantees it
			.queueCount = 1,
			.pQueuePriorities = &Priority,		
		});

	VkPhysicalDeviceFeatures DeviceFeatures{};
	
	VkDeviceCreateInfo CreateInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size()),
		.pQueueCreateInfos = QueueCreateInfos.data(),
		.enabledLayerCount = static_cast<uint32_t>(RequiredLayers.size()),
		.ppEnabledLayerNames = RequiredLayers.size() ? RequiredLayers.data() : nullptr,
		.enabledExtensionCount = static_cast<uint32_t>(RequiredDeviceExtensions.size()),
		.ppEnabledExtensionNames = RequiredDeviceExtensions.size() ? RequiredDeviceExtensions.data() : nullptr,
		.pEnabledFeatures = &DeviceFeatures,
	};

	VkDevice Device;
	if(vkCreateDevice(PhyDevice.m_Device, &CreateInfo, nullptr, &Device) != VK_SUCCESS)
		throw std::runtime_error{"Couldn't create logical device"};

	m_Device.emplace(std::move(Device), m_PickedPhyDevice.value());
}

void CCore::CreateSwapChain(CWindow &Window)
{
	m_Swapchain.emplace(m_Device.value(), Window, m_Surface);
}

void CCore::CreateRenderPass()
{
	m_RenderPass.emplace(m_Device.value(), m_Swapchain.value());
}

void CCore::CreatePipeline(std::string VertexShaderPath, std::string FragmentShaderPath)
{
	auto VertexShader = CShader(m_Device.value(), VertexShaderPath.c_str());
	auto FragmentShader = CShader(m_Device.value(), FragmentShaderPath.c_str());
	m_Pipeline.emplace(std::move(VertexShader), std::move(FragmentShader), m_Device.value(), m_Swapchain.value(), m_RenderPass.value());
}

void CCore::CreateFramebuffers()
{
	m_RenderPass->CreateFramebuffers(m_Swapchain.value());
}

void CCore::CreateCommandBuffer()
{
	VkCommandPoolCreateInfo PoolCreateInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = m_Device->PhyDevice().m_GraphicsQueueFamily.value(),
	};

	VkCommandPool Pool;
	if(vkCreateCommandPool(m_Device->Device(), &PoolCreateInfo, nullptr, &Pool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool");
	m_CommandPool.emplace(std::move(Pool));

	m_CommandBuffer.emplace(m_Device.value(), m_CommandPool.value());
}

void CCore::CreateSyncObjects()
{
	VkSemaphoreCreateInfo SemCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};
	VkFenceCreateInfo FenceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	if(vkCreateSemaphore(m_Device->Device(), &SemCreateInfo, nullptr, &m_ImageAvailable) != VK_SUCCESS ||
		vkCreateSemaphore(m_Device->Device(), &SemCreateInfo, nullptr, &m_RenderFinished) != VK_SUCCESS ||
		vkCreateFence(m_Device->Device(), &FenceCreateInfo, nullptr, &m_InFlight) != VK_SUCCESS)
		throw std::runtime_error{"Failed to create synchronization primitives"};

}

void CCore::WaitIdle()
{
	vkWaitForFences(m_Device->Device(), 1, &m_InFlight, VK_TRUE, UINT64_MAX);
	vkResetFences(m_Device->Device(), 1, &m_InFlight);
}

void CCore::Draw()
{
	uint32_t ImageIndex;
	vkAcquireNextImageKHR(m_Device->Device(), m_Swapchain->SwapChain(), UINT64_MAX, m_ImageAvailable, VK_NULL_HANDLE, &ImageIndex);
	m_CommandBuffer->Reset();
	m_CommandBuffer->Record(ImageIndex, m_RenderPass.value(), m_Pipeline.value(), m_Swapchain.value());

	VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSubmitInfo SubmitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_ImageAvailable,
		.pWaitDstStageMask = WaitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_CommandBuffer->CommandBuffer(),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &m_RenderFinished,
	};

	if(vkQueueSubmit(m_Device->GraphicsQueue(), 1, &SubmitInfo, m_InFlight) != VK_SUCCESS)
		throw std::runtime_error{"Failed to submit draw command buffer"};

	VkPresentInfoKHR PresentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &m_RenderFinished,
		.swapchainCount = 1,
		.pSwapchains = &m_Swapchain->SwapChain(),
		.pImageIndices = &ImageIndex,
		.pResults = nullptr,
	};

	vkQueuePresentKHR(m_Device->PresentQueue(), &PresentInfo);
}

}
