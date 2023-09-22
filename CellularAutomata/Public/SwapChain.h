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

	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	static bool DeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

public:
	SwapChain() = delete;
	SwapChain(Application* app);
	~SwapChain();

	VkSwapchainKHR Get() { return m_swapChain; }

protected:
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D SelectSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateImageViews();

private:
	// Handles
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	VkDevice m_device;
	// Metadata
	VkFormat m_format;
	VkExtent2D m_extent;

};