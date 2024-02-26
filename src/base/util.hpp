#ifndef INCLUDE_BASE_UTIL_HPP_
#define INCLUDE_BASE_UTIL_HPP_

#include <vector>
#include <algorithm>
#include <fstream>
#include <memory>
#include <format>

namespace vkexperiment::base::util {

template <typename T>
void dedupe(std::vector<T> &vec)
{
	std::ranges::sort(vec);
	const auto [first, last] = std::ranges::unique(vec);
	vec.erase(first, last);
}


inline auto readfile(const char *filepath)
{
	std::ifstream f{filepath, std::ios::ate | std::ios::binary};
	if(!f)
		throw std::runtime_error{std::format("Couldn't open file: {}", filepath)};


	size_t Size = f.tellg();
	f.seekg(0);

	auto buf = std::make_unique_for_overwrite<char[]>(Size);
	f.read(buf.get(), Size);
	
	if(f.gcount() != Size)
		throw std::runtime_error{"Shader file changed"};

	return std::make_tuple(std::move(buf), Size);
}

}

#endif  // INCLUDE_BASE_UTIL_HPP_
