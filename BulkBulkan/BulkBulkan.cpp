#include "Application/TriangleApp.h"
#include <iostream>
#include <stdexcept>

int main() {
	BulkBulkan::TriangleApp app{};

	try {
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}