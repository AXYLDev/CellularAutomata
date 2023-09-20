#include "Application.h"
#include <set>

/* ---- Vulkan Wrappers ---- */
VkResult Application::VkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void Application::VkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

/* ---- Init ---- */
// GLFW init
void Application::InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(s_WindowWidth, s_WindowHeight, "Cellular Automata", nullptr, nullptr);

}

// Instance init
void Application::InitVulkan() {
	CreateInstance();
	InitDebug();
	CreateSurface();
	SelectPhysicalDevice();
	InitLogicalDevice();
}
void Application::CreateInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Cellular Automata";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.pEngineName = "Vulkan Test";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Extensions
	std::vector<const char*> extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = (uint32_t)extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
	// Debug
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (s_EnableValidationLayers) {
		if (!CheckValidationLayerSupport()) {
			std::cout << "Validation layers not supported.";
			throw;
		}
		createInfo.enabledLayerCount = _countof(s_ValidationLayers);
		createInfo.ppEnabledLayerNames = s_ValidationLayers;
		InitDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
		std::cout << "Failed to create instance.";
		throw;
	}
}
bool Application::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (const char* layerName : s_ValidationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		if (!layerFound) return false;
	}
	return true;
}
std::vector<const char*> Application::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (s_EnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

// Window surface init
void Application::CreateSurface() {
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
		std::cout << "Failed to create window surface";
		throw;
	}
}

// Debug init
void Application::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = s_DebugCallback;
	createInfo.pUserData = nullptr;
}
void Application::InitDebug() {
	if (!s_EnableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	InitDebugMessengerCreateInfo(createInfo);
	if (VkCreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
		std::cout << "Failed to create debug messenger.";
		throw;
	}
}

// Device selection/creation
void Application::SelectPhysicalDevice() {
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		std::cout << "No GPUs supporting Vulkan found.";
		throw;
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
	// Prefer discrete GPU with highest score
	uint32_t maxScore = 0, bool discreteFound = false;
	for (const auto& device : devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		// Check support
		{
			// Queue family support
			if (!FindQueueFamilies(device).FoundAll()) continue;
			
			// Extension support
			uint32_t extCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExtensions.data());
			std::set<std::string> requiredExtensions(s_Extensions, s_Extensions + _countof(s_Extensions));
			for (const auto& ext : availableExtensions) {
				requiredExtensions.erase(ext.extensionName);
			}
			// Check required extension not present
			if (!requiredExtensions.empty()) continue;
			
			// Check swap chain support
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) continue;
		}

		// Find score
		bool discrete = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		if (!discreteFound || discrete) {
			uint32_t score = deviceProperties.limits.maxImageDimension2D;
			if (score > maxScore) {
				m_physicalDevice = device;
				score = maxScore;
				discreteFound = discrete;
			}
		}
	}
}
Application::QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice device) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	QueueFamilyIndices indices;
	for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphics = i;
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (presentSupport)
			indices.present = i;
	}
	return indices;
}
void Application::InitLogicalDevice() {
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

	// Request required queue handles
	uint32_t queueFamilies[] = {indices.graphics, indices.present};
	VkDeviceQueueCreateInfo queueCreateInfos[_countof(queueFamilies)] = {};
	for (uint32_t i = 0; i < _countof(queueFamilies); i++) {
		VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[i];
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilies[i];
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
	}
	// Required features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.queueCreateInfoCount = _countof(queueCreateInfos);
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.ppEnabledExtensionNames = s_Extensions;
	deviceCreateInfo.enabledExtensionCount = _countof(s_Extensions);
	if (s_EnableValidationLayers) {
		deviceCreateInfo.enabledLayerCount = _countof(s_ValidationLayers);
		deviceCreateInfo.ppEnabledLayerNames = s_ValidationLayers;
	}
	else deviceCreateInfo.enabledLayerCount = 0;

	if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
		std::cout << "Failed to create logical device.";
		throw;
	}

	// Get queue
	vkGetDeviceQueue(m_device, indices.graphics, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.present, 0, &m_presentQueue);
}
Application::SwapChainSupportDetails Application::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}


/* ---- Application Loop ---- */
void Application::Run() {
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
	}
}

/* ---- Cleanup ---- */
Application::~Application() {
	vkDestroyDevice(m_device, nullptr);
	if (s_EnableValidationLayers) {
		VkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}