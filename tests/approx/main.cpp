#include "engine.hpp"

#include <gl/glew.h>

#include "nanovg.h"
#include "imgui.h"
#include "nvg.hpp"

#include <ez/interpolate/PointCloud.hpp>

class Approx: public Engine {
public:
	using Knot = ez::PointCloud<float, 1, 1>::Point;

	Approx()
		: Engine("Approximation test")
	{
		vg = nvgCreate();

		glClearColor(0.9, 0.9, 0.9, 1);

		interp.append(-1 * ez::pi<float>(), -1);
		interp.append( 0 * ez::pi<float>(), 1);
		interp.append( 1 * ez::pi<float>(), -1);
		interp.append( 2 * ez::pi<float>(), 1);
	}
	~Approx() {
		nvgDelete(vg);
	}

	void handleEvent(const ez::InputEvent& ev) override {
		//inputMan.handleEvent(ev);
	}
	void gui() override {
		ImGui::Begin("Knot");
		Knot &knot = interp[0];

		if (ImGui::DragFloat("Value", &knot.domain, 1E-3f, 0.f, 2.f)) {
			
		}

		ImGui::End();
	}
	void render() override {
		glm::vec2 dims = getWindowSize();
		glm::vec2 frame = getFrameSize();

		glClearColor(217 / 255.f, 245 / 255.f, 255 / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(vg, dims.x, dims.y, frame.x / dims.x);

		nvgTranslate(vg, dims.x / 2.f, dims.y / 2.f);
		nvgScale(vg, 50, 50);
		nvgStrokeWidth(vg, 1.f / 50.f);

		float input = -ez::tau<float>();
		float value = interp.eval(input);

		nvgBeginPath(vg);
		nvgMoveTo(vg, input, value);
		for (input += 0.1f; input < ez::tau<float>(); input += 0.1f) {
			value = interp.eval(input);
			nvgLineTo(vg, input, value);
		}
		nvgStroke(vg);

		input = -ez::tau<float>();
		value = std::cos(input);
		nvgBeginPath(vg);
		nvgMoveTo(vg, input, value);
		for (input += 0.1f; input < ez::tau<float>(); input += 0.1f) {
			value = std::cos(input);
			nvgLineTo(vg, input, value);
		}
		nvgStrokeColor(vg, nvgRGBf(1, 0, 0));
		nvgStroke(vg);


		nvgEndFrame(vg);


		glDisable(GL_BLEND);
	}

	ez::PointCloud<float, 1, 1> interp;
	NVGcontext * vg;
};

int main() {
	Approx engine;

	return engine.run();
}