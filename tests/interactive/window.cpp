#include "window.hpp"
#include <ez/math/constants.hpp>

#include <ez/gl.hpp>
#include <GL/glew.h>

struct NVGcontext;
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "imgui.h"

Window::Window(std::string_view _title, glm::ivec2 size, ez::window::Style _style, const ez::window::RenderSettings& rs)
	: ez::window::Window(_title, size, _style, rs)
	, gui(context)
	, nvgContext(nullptr)
	, imguiContext(*this)
{	
	ops.add(new AddPoint{});
	ops.add(new DragPoint{});
	ops.add(new DragTangent{});
	ops.add(new Select{});

	inputMan.setKeyMap(keymap);

	keymap.add(ez::KeyMods::None, ez::Key::T, "drag_tangent");
	keymap.add(ez::KeyMods::None, ez::Key::G, "drag_point");
	keymap.add(ez::KeyMods::None, ez::Key::None, "select");
	keymap.add(ez::KeyMods::None, ez::Key::A, "add_point");

	setActive(true);
	ez::gl::load();

	nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

	ImGui::StyleColorsDark();
}
Window::~Window()
{}

void Window::handleInput() {
	imguiContext.makeActive();

	ez::InputEvent ev;
	while (pollInput(ev)) {
		imguiContext.processEvent(ev);

		inputMan.handleEvent(ev, context, ops);

		if (ev.type == ez::InEv::Closed) {
			close();
		}
	}
}
void Window::draw() {
	imguiContext.makeActive();
	imguiContext.newFrame(*this);

	if (ImGui::Begin("Edit")) {
		gui.draw();
	}
	ImGui::End();

	glm::vec2 window = getSize();
	glm::vec2 frame = getViewportSize();

	NVGcontext* vg = nvgContext;
	context.buildCurve();
	const std::vector<glm::vec2>& curve = context.getCurve();

	glClearColor(0.9, 0.9, 0.9, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(vg, window.x, window.y, frame.x / window.x);

	if (curve.size() > 0) {
		nvgBeginPath(vg);
		nvgStrokeColor(vg, nvgRGBf(0, 0, 1));
		nvgStrokeWidth(vg, 5.f);

		glm::vec2 point = curve.front();

		nvgMoveTo(vg, point.x, point.y);
		for (int i = 1; i < curve.size(); ++i) {
			point = curve[i];
			nvgLineTo(vg, point.x, point.y);
		}
		nvgStroke(vg);
	}

	if (context.hasPointSelect()) {
		ez::PointCloud<glm::vec2>& reduce = context.getReduction();

		nvgFillColor(vg, nvgRGBf(0, 1, 0));

		for (std::ptrdiff_t i = 0; i < reduce.numPoints(); ++i) {
			glm::vec2 point = reduce.getPoint(i);

			nvgBeginPath(vg);
			nvgArc(vg, point.x, point.y, 5.f, 0, ez::tau<float>(), NVG_CCW);
			nvgFill(vg);
		}

		if (context.hasInputSelect()) {
			std::ptrdiff_t inputSelect = context.getInputSelect();

			nvgStrokeColor(vg, nvgRGBf(0.6f, 0.3, 0.7f));
			nvgStrokeWidth(vg, 3.f);

			for (std::ptrdiff_t i = 0; i < reduce.numPoints(); ++i) {
				glm::vec2 tangent = reduce.getKnot(i, inputSelect).tangent;
				glm::vec2 point = reduce.getPoint(i);

				nvgBeginPath(vg);
				nvgMoveTo(vg, point.x, point.y);
				nvgLineTo(vg, point.x + tangent.x, point.y + tangent.y);
				nvgStroke(vg);
			}
		}

		std::ptrdiff_t select = context.getControlSelect();
		if (select != -1) {
			glm::vec2 point = reduce.getPoint(select);

			nvgBeginPath(vg);
			nvgStrokeWidth(vg, 2.f);
			nvgStrokeColor(vg, nvgRGB(255, 0, 0));
			nvgArc(vg, point.x, point.y, 7.f, 0, ez::tau<float>(), NVG_CCW);
			nvgStroke(vg);
		}
	}

	nvgEndFrame(vg);

	imguiContext.render();

	glDisable(GL_BLEND);
}
