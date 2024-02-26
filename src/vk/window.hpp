#ifndef INCLUDE_SRC_WINDOW_HPP_
#define INCLUDE_SRC_WINDOW_HPP_

#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkexperiment::vk {
class CWindow {
	GLFWwindow *m_pWindow;
	
	const int m_Width;
	const int m_Height;

	std::string m_Name;
	
public:
	CWindow(int w, int h, std::string name);
	~CWindow();
	
	CWindow(const CWindow &) = delete;
	CWindow &operator=(const CWindow &) = delete;

	std::tuple<int, int> FramebufferSize();

	bool ShouldClose() { return glfwWindowShouldClose(m_pWindow); }
	GLFWwindow *Window() { return m_pWindow; }
};
}

#endif  // INCLUDE_SRC_WINDOW_HPP_
