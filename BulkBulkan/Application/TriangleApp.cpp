#include "TriangleApp.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

TriangleApp::TriangleApp()
{
	initVulkan();
}

void TriangleApp::run() {
	mainLoop();
	cleanup();
}

void TriangleApp::cleanup() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void TriangleApp::mainLoop() {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
	}
}

void TriangleApp::createInstance() {
	auto appInfo = VkApplicationInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto createInfo = VkInstanceCreateInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto glfwExtensionCount = uint32_t{ 0 };
	const char** glfwExtensions{};

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	auto result = vkCreateInstance(&createInfo, nullptr, &_instance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	}
}

void TriangleApp::initVulkan() {
	createInstance();
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(WIDTH, HEIGHT, "Bulkan", nullptr, nullptr);
}