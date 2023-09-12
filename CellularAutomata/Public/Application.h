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

	static VKAPI_ATTR VkBool32 VKAPI_CALL s_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	/* ---- Vulkan Wrappers ---- */
	static VkResult VkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

public:
	Application() {
		InitWindow();
		InitVulkan();
	}
	~Application();
	void Run();

private:
	/* ---- Init ---- */
	void InitWindow();
	void InitVulkan();
	void InitDebug();
	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	GLFWwindow* m_window;
	// Vulkan Objects
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
};