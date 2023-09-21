#pragma once
#include "pch.h"

class Application;

class SwapChain {
public:
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	static bool DeviceSuitable(VkPhysicalDevice device);

public:
	SwapChain(Application* app);
	~SwapChain();

	VkSwapchainKHR Get() { return m_swapChain; }

protected:
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

private:
	VkSwapchainKHR m_swapChain;
	VkDevice m_device;

};