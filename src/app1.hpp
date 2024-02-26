#ifndef INCLUDE_SRC_APP1_HPP_
#define INCLUDE_SRC_APP1_HPP_

#include "vk/window.hpp"
#include "vk/core.hpp"

namespace vkexperiment {
class App1 {
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

public:
	vk::CWindow m_Window{WIDTH, HEIGHT, "Test"};
	vk::CCore m_Core{"App1", 0, m_Window};

	void Run();
	void Draw();
};
};

#endif  // INCLUDE_SRC_APP1_HPP_
