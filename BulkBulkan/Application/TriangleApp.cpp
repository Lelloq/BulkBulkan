#include "TriangleApp.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

const std::vector<const char*> kValidationLayers = {
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

//Validation
namespace {
	bool checkValidationSupport() {
		auto layerCount = uint32_t{ 0 };
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		auto availableLayers = std::vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto* layerName : kValidationLayers) {
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

//Device
namespace {
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		auto queueFamilyCount = 0u;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	int rateDeviceSuitability(VkPhysicalDevice device) {
		auto deviceProperties = VkPhysicalDeviceProperties{};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		auto deviceFeatures = VkPhysicalDeviceFeatures{};
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		auto score = 0;

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		score += deviceProperties.limits.maxImageDimension2D;

		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		auto indices = findQueueFamilies(device);
		if (!indices.isComplete())
		{
			return 0;
		}

		return score;
	}
}

namespace BulkBulkan {
	void TriangleApp::run() {
		while (!glfwWindowShouldClose(_window)) {
			glfwPollEvents();
		}
	}

	TriangleApp::TriangleApp()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(WIDTH, HEIGHT, "Bulkan", nullptr, nullptr);

		createInstance();
		setupDebugMessenger();
		pickPhysicalDevice();
	}

	TriangleApp::~TriangleApp()
	{
		if (ENABLE_VALIDATION_LAYERS) {
			destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
		}

		vkDestroyInstance(_instance, nullptr);
		vkDestroyDevice(_device, nullptr);

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

	void TriangleApp::pickPhysicalDevice()
	{
		auto deviceCount = 0u;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Failed to load GPUs with vulkan support");
		}

		auto devices = std::vector<VkPhysicalDevice>(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		auto candidates = std::multimap<int, VkPhysicalDevice>{};

		for (auto const& device : devices) {
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.begin()->first > 0) {
			_physicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void TriangleApp::createLogicalDevice()
	{
		auto indices = findQueueFamilies(_physicalDevice);

		auto queueCreateInfo = VkDeviceQueueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		auto queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		auto deviceFeatures = VkPhysicalDeviceFeatures{};

		auto createInfo = VkDeviceCreateInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;

		if (ENABLE_VALIDATION_LAYERS) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
			createInfo.ppEnabledLayerNames = kValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
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
			createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
			createInfo.ppEnabledLayerNames = kValidationLayers.data();

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
}