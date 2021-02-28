#pragma once
#include <ez/window/Engine.hpp>
#include <ez/input/InputEngine.hpp>
#include <ez/input/OperatorRegistry.hpp>

#include <ez/imgui/Context.hpp>

#include <ez/interpolate/PointCloud.hpp>

class Window: public ez::window::Window {
public:
	Window(std::string_view _title, glm::ivec2 size, ez::window::Style _style, const ez::window::RenderSettings& rs);
	~Window();

	void handleInput() override;
	void handleEvent(const ez::InputEvent& ev);
	void draw() override;

private:
	ez::PointCloud<float, 1, 2> pointcloud;
	int index, tangent;
	int state;

	ez::imgui::Context imguiContext;

	struct NVGcontext* nvgContext;
};