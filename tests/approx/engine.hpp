#pragma once
#include <ez/imgui/ImGuiEngine.hpp>
#include <ez/input/InputEngine.hpp>

#include <ez/interpolate/PointCloud.hpp>

class Engine: public ImGuiEngine {
public:
	Engine();

	void postInitialize();
	void handleEvent(const ez::InputEvent& ev);
	void buildGui();
	void render();
private:
	ez::InputEngine inputMan;
	ez::KeyMap keymap;

	ez::PointCloud<float, 1> interp;
};