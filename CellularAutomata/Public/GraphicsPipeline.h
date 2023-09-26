#pragma once
#include "pch.h"
class Application;

class GraphicsPipeline {
public:
	static std::vector<char> ReadFile(const std::string& filePath);

public:
	GraphicsPipeline() = delete;
	GraphicsPipeline(Application* app);
	~GraphicsPipeline();

protected:
	VkShaderModule CreateShaderModule(const std::string& filePath);

private:
	Application* m_app;

};