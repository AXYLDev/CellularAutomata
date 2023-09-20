#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

class Application {
public:
	/* ---- Static Constexpr ---- */
	static constexpr uint32_t s_WindowWidth = 800;
	static constexpr uint32_t s_WindowHeight = 600;

#ifdef NDEBUG
	static constexpr bool s_EnableValidationLayers = false;
#else
	static constexpr bool s_EnableValidationLayers = true;
#endif
	static constexpr const char* s_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};
	static constexpr const char* s_Extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL s_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	/* ---- Vulkan Wrappers ---- */
	static VkResult VkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void VkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

public:
	Application() {
		InitWindow();
		InitVulkan();
	}
	~Application();
	void Run();

private:
	/* ---- Init ---- */
	// GLFW init
	void InitWindow();
	// Instance init
	void InitVulkan();
	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	// Window surface init
	void CreateSurface();
	// Debug init
	void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void InitDebug();
	// Device selection/creation
	void SelectPhysicalDevice();
	struct QueueFamilyIndices {
		uint32_t graphics = UINT32_MAX;
		uint32_t present = UINT32_MAX;
		bool FoundAll() {
			return graphics != UINT32_MAX && present != UINT32_MAX;
		}
	};
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	void InitLogicalDevice();
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	GLFWwindow* m_window;
	// Vulkan Objects
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
};