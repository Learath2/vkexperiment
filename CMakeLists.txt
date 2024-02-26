cmake_minimum_required(VERSION 3.15)
project(vkexperiment VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(src/version.hpp.in version.hpp)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)

add_custom_command(OUTPUT shaders/vert.spv
	COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o shaders/vert.spv ${PROJECT_SOURCE_DIR}/shaders/shader.vert
	DEPENDS ${PROJECT_SOURCE_DIR}/shaders/shader.vert)

add_custom_command(OUTPUT shaders/frag.spv
	COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o shaders/frag.spv ${PROJECT_SOURCE_DIR}/shaders/shader.frag
	DEPENDS ${PROJECT_SOURCE_DIR}/shaders/shader.frag)

add_executable(vkexperiment src/main.cpp src/app1.cpp src/vk/window.cpp src/vk/shader.cpp
	src/vk/core.cpp src/vk/swapchain.cpp src/vk/renderpass.cpp src/vk/pipeline.cpp
	src/vk/commandbuffer.cpp
	shaders/vert.spv shaders/frag.spv)
target_link_libraries(vkexperiment glfw)
target_link_libraries(vkexperiment Vulkan::Vulkan)
target_include_directories(vkexperiment PUBLIC "${PROJECT_BINARY_DIR}" "src/")
