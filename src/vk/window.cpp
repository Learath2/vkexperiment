#include "window.hpp"

namespace vkexperiment::vk {
CWindow::CWindow(int h, int w, std::string name) : m_Height(h), m_Width(w), m_Name(name), m_pWindow(nullptr)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_pWindow = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
}

CWindow::~CWindow()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

std::tuple<int, int> CWindow::FramebufferSize()
{
	std::tuple<int, int> Result;
	glfwGetFramebufferSize(m_pWindow, &std::get<0>(Result), &std::get<1>(Result));
	return Result;
}

}
