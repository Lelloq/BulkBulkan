#include "TriangleApp.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <vector>

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

//Statics (may move them soon as the project gets larger)
namespace {
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
}

namespace {
	bool checkValidationSupport() {
		auto layerCount = uint32_t{ 0 };
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		auto availableLayers = std::vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto* layerName : validationLayers) {
			auto foundLayer = false;

			for (const auto& layerProps : availableLayers) {
				if (strcmp(layerName, layerProps.layerName) == 0) {
					foundLayer = true;
					break;
				}
			}
			if (!foundLayer) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions() {
		auto glfwExtensionCount = uint32_t{ 0 };
		const char** glfwExtensions{};
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		auto extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (ENABLE_VALIDATION_LAYERS) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}
}

namespace BulkBulkan {
	void TriangleApp::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void TriangleApp::cleanup()
	{
		if (ENABLE_VALIDATION_LAYERS) {
			destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
		}

		vkDestroyInstance(_instance, nullptr);
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void TriangleApp::setupDebugMessenger()
	{
		if (!ENABLE_VALIDATION_LAYERS) return;

		auto createInfo = VkDebugUtilsMessengerCreateInfoEXT{};
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger");
		}
	}

	void TriangleApp::createInstance() {
		if (ENABLE_VALIDATION_LAYERS and !checkValidationSupport()) {
			throw std::runtime_error("validation layers are requested, but not available");
		}

		auto appInfo = VkApplicationInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		auto createInfo = VkInstanceCreateInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		auto debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		debugCreateInfo.pUserData = nullptr;

		if (ENABLE_VALIDATION_LAYERS) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		auto result = vkCreateInstance(&createInfo, nullptr, &_instance);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance");
		}

	}

	void TriangleApp::initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(WIDTH, HEIGHT, "Bulkan", nullptr, nullptr);
	}

	void TriangleApp::initVulkan() {
		createInstance();
		setupDebugMessenger();
	}

	void TriangleApp::mainLoop() {
		while (!glfwWindowShouldClose(_window)) {
			glfwPollEvents();
		}
	}
}