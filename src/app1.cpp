#include "app1.hpp"

namespace vkexperiment {
void App1::Run()
{
	while(!m_Window.ShouldClose()) {
		glfwPollEvents();
		Draw();
	}
}

void App1::Draw()
{
	m_Core.WaitIdle();
	m_Core.Draw();
}

}
