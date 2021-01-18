#pragma once
#include <ez/imgui/ImGuiEngine.hpp>
#include <ez/input/InputEngine.hpp>
#include "context.hpp"
#include "gui.hpp"

#include "ops/AddPoint.hpp"
#include "ops/DragPoint.hpp"
#include "ops/DragTangent.hpp"
#include "ops/Select.hpp"

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

	Context context;
	GUI gui;

	AddPoint addPoint;
	DragPoint dragPoint;
	DragTangent dragTangent;
	Select select;
};