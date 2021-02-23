#include "engine.hpp"
#include <RenderTools/RenderTools.hpp>
#include <ez/math/MathConstants.hpp>
#include "nanovg.h"
#include "imgui.h"
#include <ez/math/MathConstants.hpp>

Engine::Engine()
{
}

void Engine::postInitialize() {
	glClearColor(0.9, 0.9, 0.9, 1);

	interp.resizeInputs(1);
	
	interp.append(1);
	interp.append(-1);
	interp.append(1);
	interp.append(-1);
	interp.append(1);

	using Knot = ez::PointCloud<float, 1>::Knot;
	Knot knot;
	knot.position = 0;
	knot.domain = 0.5f;

	interp.setKnot(2, 0, knot);
	knot.domain = 1.f;
	
	knot.position = -ez::PI<float>();
	interp.setKnot(1, 0, knot);

	knot.position = ez::PI<float>();
	interp.setKnot(3, 0, knot);

	knot.position = -ez::TAU<float>();
	interp.setKnot(0, 0, knot);

	knot.position = ez::TAU<float>();
	interp.setKnot(4, 0, knot);
}
void Engine::handleEvent(const ez::InputEvent& ev) {
	inputMan.handleEvent(ev);
}
void Engine::buildGui() {
	ImGui::Begin("Knot");

	using Knot = ez::PointCloud<float, 1>::Knot;
	Knot knot = interp.getKnot(2, 0);

	if (ImGui::DragFloat("Value", &knot.domain, 1E-3f, 0.f, 2.f)) {
		interp.setKnot(2, 0, knot);
	}

	ImGui::End();
}
void Engine::render() {
	glm::vec2 window = getWindowSize();
	glm::vec2 frame = getFrameSize();

	NVGcontext* vg = getNVGContext();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	nvgBeginFrame(vg, window.x, window.y, frame.x / window.x);
	
	nvgTranslate(vg, window.x / 2.f, window.y / 2.f);
	nvgScale(vg, 50, 50);
	nvgStrokeWidth(vg, 1.f / 50.f);

	float input = -ez::TAU<float>();
	float value = interp.eval(&input, &input + 1);

	nvgBeginPath(vg);
	nvgMoveTo(vg, input, value);
	for (input += 0.1f; input < ez::TAU<float>(); input += 0.1f) {
		value = interp.eval(&input, &input + 1);
		nvgLineTo(vg, input, value);
	}
	nvgStroke(vg);

	input = -ez::TAU<float>();
	value = std::cos(input);
	nvgBeginPath(vg);
	nvgMoveTo(vg, input, value);
	for (input += 0.1f; input < ez::TAU<float>(); input += 0.1f) {
		value = std::cos(input);
		nvgLineTo(vg, input, value);
	}
	nvgStrokeColor(vg, nvgRGBf(1, 0, 0));
	nvgStroke(vg);


	nvgEndFrame(vg);
	
	
	glDisable(GL_BLEND);
}