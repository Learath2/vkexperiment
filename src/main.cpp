#include "app1.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main (int argc, char *argv[]) {
	vkexperiment::App1 app{};

	try {
		app.Run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
