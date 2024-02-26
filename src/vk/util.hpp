#ifndef INCLUDE_VK_UTIL_HPP_
#define INCLUDE_VK_UTIL_HPP_

#include <cstdint>
#include <concepts>
#include <stdexcept>
#include <vector>
#include <format>

#include <vulkan/vulkan_core.h>

#define UTIL_VK_ENUMERATE_HELPER(Fn, OutType) UtilVkEnumerate<OutType>(Fn, #Fn)

namespace vkexperiment::vk {

template<typename T>
concept VkEnumFn = requires(T f, uint32_t c) { f(c, nullptr); };

template<typename T, std::invocable<uint32_t *, T *> VkFn>
std::vector<T> UtilVkEnumerate(VkFn &&F, const char *FName)
{
	uint32_t Count = 0;
	if(F(&Count, nullptr) != VK_SUCCESS)
		throw std::runtime_error{std::format("Enumerate failed {}", FName)};

	std::vector<T> Result(Count);
	if(F(&Count, Result.data()) != VK_SUCCESS)
		throw std::runtime_error{std::format("Enumerate failed {}", FName)};

	return Result;
}

}

#endif  // INCLUDE_VK_UTIL_HPP_
