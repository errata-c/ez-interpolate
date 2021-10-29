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
		interp.append(-1 * ez::pi<float>(), -1);
		interp.append( 0 * ez::pi<float>(), 1);
		interp.append( 1 * ez::pi<float>(), -1);
		interp.append( 2 * ez::pi<float>(), 1);
	}
	~Approx() {
	}

	void handleEvent(const ez::InputEvent& ev) override {
		
	}
	void gui() override {
		ImGui::Begin("Knot");
		Knot &knot = interp[0];

		ImGui::DragFloat("Value", &knot.domain, 1E-3f, 0.f, 2.f);

		ImGui::End();
	}
	void render() override {
		glm::vec2 dims = getWindowSize();
		glm::vec2 frame = getFrameSize();

		translate(dims / 2.f);

		scale(50.f);
		strokeWidth(1.f / 50.f);

		float input = -ez::tau<float>();
		float value = interp.eval(input);

		beginPath();
		moveTo({input, value});
		for (input += 0.1f; input < ez::tau<float>(); input += 0.1f) {
			value = interp.eval(input);
			pathTo({input, value});
		}
		stroke();

		input = -ez::tau<float>();
		value = std::cos(input);
		beginPath();
		moveTo({input, value});
		for (input += 0.1f; input < ez::tau<float>(); input += 0.1f) {
			value = std::cos(input);
			pathTo({input, value});
		}

		strokeColor(1.f, 0.f, 0.f);
		stroke();
	}

	ez::PointCloud<float, 1, 1> interp;
};

int main() {
	Approx engine;

	return engine.run();
}