#include "engine.hpp"
#include <RenderTools/RenderTools.hpp>
#include <ez/math/MathConstants.hpp>
#include "nanovg.h"
#include "imgui.h"

Engine::Engine()
	: gui(context)
	, addPoint(context)
	, dragPoint(context)
	, dragTangent(context)
	, select(context)
{
	inputMan.setKeyMap(keymap);

	keymap.add(ez::KeyData(ez::Key::T), dragTangent);
	keymap.add(ez::KeyData(ez::Key::G), dragPoint);
	keymap.add(ez::KeyData(ez::Key::None), select);
	keymap.add(ez::KeyData(ez::Key::A), addPoint);
}

void Engine::postInitialize() {
	glClearColor(0.9, 0.9, 0.9, 1);
}
void Engine::handleEvent(const ez::InputEvent& ev) {
	inputMan.handleEvent(ev);
}
void Engine::buildGui() {
	if (ImGui::Begin("Edit")) {
		gui.draw();
	}
	ImGui::End();
}
void Engine::render() {
	glm::vec2 window = getWindowSize();
	glm::vec2 frame = getFrameSize();

	NVGcontext* vg = getNVGContext();
	context.buildCurve();
	const std::vector<glm::vec2>& curve = context.getCurve();

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
		ez::PointCloud<float, 2>& reduce = context.getReduction();

		nvgFillColor(vg, nvgRGBf(0, 1, 0));

		for (std::ptrdiff_t i = 0; i < reduce.numPoints(); ++i) {
			glm::vec2 point = reduce.getPoint(i);

			nvgBeginPath(vg);
			nvgArc(vg, point.x, point.y, 5.f, 0, ez::TAU<float>(), NVG_CCW);
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
			nvgArc(vg, point.x, point.y, 7.f, 0, ez::TAU<float>(), NVG_CCW);
			nvgStroke(vg);
		}
	}

	nvgEndFrame(vg);
	
	
	glDisable(GL_BLEND);
}