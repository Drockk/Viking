#pragma once

#include "Viking/Core/Application.hpp"

#include <iostream>

extern std::unique_ptr<Viking::Application> createApplication();

int main() {
	try {
		const auto app = createApplication();
		app->init();
		app->run();
		app->shutdown();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}