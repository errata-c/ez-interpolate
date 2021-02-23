#pragma once
#include <ez/window/Engine.hpp>
#include <ez/input/InputEngine.hpp>
#include <ez/input/OperatorRegistry.hpp>

#include <ez/imgui/Context.hpp>

#include "context.hpp"
#include "gui.hpp"

#include "ops/AddPoint.hpp"
#include "ops/DragPoint.hpp"
#include "ops/DragTangent.hpp"
#include "ops/Select.hpp"

class Window: public ez::window::Window {
public:
	Window(std::string_view _title, glm::ivec2 size, ez::window::Style _style, const ez::window::RenderSettings& rs);
	~Window();

	void handleInput() override;
	void draw() override;
private:
	ez::InputEngine inputMan;
	ez::KeyMap keymap;
	ez::OperatorRegistry<Context> ops;

	ez::imgui::Context imguiContext;

	struct NVGcontext* nvgContext;
	Context context;
	GUI gui;
};