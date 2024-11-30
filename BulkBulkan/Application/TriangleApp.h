#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily{};

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

namespace BulkBulkan {
	class TriangleApp {
	public:
		void run();

	private:
		void createInstance();
		void initWindow();
		void initVulkan();
		void mainLoop();
		void cleanup();
		void setupDebugMessenger();
		void pickPhysicalDevice();

		VkInstance _instance{};
		GLFWwindow* _window{};
		VkDebugUtilsMessengerEXT _debugMessenger{};
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	};
}