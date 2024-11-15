#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

class TriangleApp {
public:
	TriangleApp();

	void run();
	void createInstance();
	void initVulkan();
	void mainLoop();
	void cleanup();

private:
	VkInstance _instance{};
	GLFWwindow* _window{};
};