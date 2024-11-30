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

		TriangleApp();
		~TriangleApp();
	private:
		void createInstance();
		void setupDebugMessenger();
		void pickPhysicalDevice();
		void createLogicalDevice();

		GLFWwindow* _window{};

		VkQueue _graphicsQueue{};
		VkDevice _device{};
		VkInstance _instance{};
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT _debugMessenger{};
	};
}