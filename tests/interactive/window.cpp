#include "window.hpp"
#include <ez/math/constants.hpp>

#include <rt/loader.hpp>

struct NVGcontext;
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "imgui.h"

enum State {
	None,
	MovingPoint,
	MovingTangent,
};

Window::Window(std::string_view _title, glm::ivec2 size, ez::window::Style _style, const ez::window::RenderSettings& rs)
	: ez::window::Window(_title, size, _style, rs)
	, nvgContext(nullptr)
	, index{0}
	, tangent(0)
	, state(State::None)
{
	setActive(true);
	rt::load();

	nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

	ImGui::StyleColorsDark();

	pointcloud.append(0.f,   glm::vec2{ 100, 100 });
	pointcloud.append(0.25f, glm::vec2{ 200, 200 });
	pointcloud.append(0.5f,  glm::vec2{ 300, 300 });
	pointcloud.append(0.75f, glm::vec2{ 400, 400 });
	pointcloud.append(1.f,   glm::vec2{ 500, 500 });
}
Window::~Window()
{}

void Window::handleInput() {
	imguiContext.makeActive();

	ez::InputEvent ev;
	while (pollInput(ev)) {
		imguiContext.processEvent(ev);
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
			handleEvent(ev);
		}

		if (ev.type == ez::InEv::Closed) {
			close();
		}
	}
}
void Window::handleEvent(const ez::InputEvent& ev) {
	if (index == pointcloud.size()) {
		return;
	}

	if (ev.type == ez::InEv::MousePress) {
		if (state == State::None) {
			if (ev.mouse.button == ez::Mouse::Left) {
				state = State::MovingPoint;
			}
			else if(ev.mouse.button == ez::Mouse::Right) {
				state = State::MovingTangent;
			}
		}
	}
	else if(ev.type == ez::InEv::MouseRelease) {
		if (state == State::MovingPoint && ev.mouse.button == ez::Mouse::Left) {
			state = State::None;
		}
		else if (state == State::MovingTangent && ev.mouse.button == ez::Mouse::Right) {
			state = State::None;
		}
	}
	else if(ev.type == ez::InEv::MouseMove) {
		if (state == State::MovingPoint) {
			pointcloud[index].output = ev.mouse.position;
		}
		else if (state == State::MovingTangent) {
			pointcloud[index].tangent[0] = 3.f * (glm::vec2{ ev.mouse.position } - pointcloud[index].output);
		}
	}
}
void Window::draw() {
	imguiContext.makeActive();
	imguiContext.newFrame(*this);

	if (ImGui::Begin("Edit")) {
		ImGui::Text("Left click anywhere in the scene to move the active point.");
		ImGui::Text("Right click to drag out a tangent from the active point.");

		ImGui::DragInt("Index", &index, 0.0625f, 0, pointcloud.size());

		if (index != pointcloud.size()) {
			ImGui::DragFloat("domain", &pointcloud[index].domain, 0.5f, 1e-5f, 100.f, "%.4f", ImGuiSliderFlags_Logarithmic);
		}
	}
	ImGui::End();

	glm::vec2 window = getSize();
	glm::vec2 frame = getViewportSize();

	NVGcontext* vg = nvgContext;

	glClearColor(0.9, 0.9, 0.9, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(vg, window.x, window.y, frame.x / window.x);

	glm::vec2 p = pointcloud.eval(0.f);
	nvgBeginPath(vg);
	nvgMoveTo(vg, p.x, p.y);

	float delta = 1.f / 100.f;
	for (float t = delta; t < 1.f; t += delta) {
		p = pointcloud.eval(t);
		nvgLineTo(vg, p.x, p.y);
	}
	p = pointcloud.eval(1.f);
	nvgLineTo(vg, p.x, p.y);

	nvgStrokeColor(vg, nvgRGB(0, 0, 0));
	nvgStroke(vg);

	if (index != pointcloud.size()) {
		glm::vec2 selected = pointcloud[index].output;

		glm::vec2 dir = pointcloud[index].tangent[0];
		if (glm::length(dir) > 6) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, selected.x, selected.y);
			glm::vec2 ep = selected + dir / 3.f;
			nvgLineTo(vg, ep.x, ep.y);
			nvgStrokeColor(vg, nvgRGB(0, 255, 0));
			nvgStroke(vg);
		}

		nvgBeginPath(vg);
		//nvgMoveTo(vg, selected.x, selected.y);
		nvgArc(vg, selected.x, selected.y, 5, 0.f, ez::tau<float>(), NVG_CW);
		nvgFillColor(vg, nvgRGB(255, 0, 0));
		nvgFill(vg);
	}

	nvgEndFrame(vg);

	imguiContext.render();

	glDisable(GL_BLEND);
}
