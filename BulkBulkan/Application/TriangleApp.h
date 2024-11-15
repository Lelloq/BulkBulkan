#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace BulkBulkan {
	class TriangleApp {
	public:
		TriangleApp();

		void run();

	private:
		void createInstance();
		void initVulkan();
		void mainLoop();
		void cleanup();
		void setupDebugMessenger();

		VkInstance _instance{};
		GLFWwindow* _window{};
		VkDebugUtilsMessengerEXT _debugMessenger{};
	};
}