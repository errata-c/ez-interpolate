#include "engine.hpp"

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>
#include <fmt/core.h>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class Offset : public Engine {
public:
	glm::vec4 taperValues;
	float offsetValue;

	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve;
	std::vector<glm::vec2> offset;

	int mode;

	Offset()
		: Engine("Bezier Offsets")
		, index(-1)
		, mode(1)
	{
		curve[0] = glm::vec2(100, 500);
		curve[1] = glm::vec2(100, 100);
		curve[2] = glm::vec2(700, 100);
		curve[3] = glm::vec2(700, 500);

		taperValues = { 0, 32, 32, 0 };
		offsetValue = 32;
	}

	void handleEvent(const ez::InputEvent& ev) override {
		if (index == -1) {
			if (ev.type == ez::InputEventType::MousePress && ev.mouse.buttons == ez::Mouse::Left) {
				glm::vec2 mpos = ev.mouse.position;

				float threshold = 12;
				std::ptrdiff_t select = -1;

				std::ptrdiff_t count = static_cast<std::ptrdiff_t>(4);
				for (std::ptrdiff_t i = 0; i < count; ++i) {
					glm::vec2 cpos = curve[i];
					float dist = glm::length(cpos - mpos);
					if (dist < threshold) {
						select = i;
						threshold = dist;
					}
				}

				index = select;
			}

			if (index != -1) {
				fmt::print("Just selected control[{}]\n", index);
			}
		}
		else {
			if (ev.type == ez::InputEventType::MouseRelease && ev.mouse.buttons == ez::Mouse::Left) {
				index = -1;
			}
			else if (ev.type == ez::InputEventType::MouseMove) {
				glm::vec2 mpos = ev.mouse.position;

				curve[index] = mpos;
			}
		}
	}

	void gui() override {
		ImGui::Begin("Edit");

		ImGui::RadioButton("Taper", &mode, 1);
		ImGui::DragFloat4("Tapers", &taperValues[0], 1.f, 0.f, 128.f, "%.0f", 1.f);
		ImGui::RadioButton("Normal", &mode, 2);
		ImGui::DragFloat("Offset", &offsetValue, 1.f, 0.f, 128.f, "%.0f", 1.f);
		

		ImGui::End();
	}

	void render() override {
		float t;
		if (ez::bezier::findCusp(curve[0], curve[1], curve[2], curve[3], t)) {
			glm::vec2 cusp = ez::bezier::interpolateStatic<4>(curve.begin(), t);
			
			fillColor(0.1, 0.1, 0.1);
			beginPath();
			circle(cusp, 12.f);
			fill();
		}

		strokeColor(0, 0, 0);
		fillColor(0.2, 0.2, 0.2);
		strokeWidth(3);

		beginPath();
		moveTo(curve[0]);
		pathTo(curve[1], curve[2], curve[3]);
		stroke();

		offset.clear();
		std::ptrdiff_t count = 0;
		if (mode == 1) {
			count = ez::bezier::taperedPixelOffset(curve[0], curve[1], curve[2], curve[3], taperValues, std::back_inserter(offset));
		}
		else {
			count = ez::bezier::pixelOffset(curve[0], curve[1], curve[2], curve[3], offsetValue, std::back_inserter(offset));
		}
		assert(count == offset.size());

		if (offset.size() > 0) {
			beginPath();
			for (std::ptrdiff_t i = 0; i < std::ptrdiff_t(offset.size()); i += 4) {
				moveTo(offset[i]);
				pathTo(offset[i + 1], offset[i + 2], offset[i + 3]);
			}
			stroke();
		}

		for (int i = 0; i < 4; ++i) {
			if (i == index) {
				fillColor(0, 0, 1);
			}
			else {
				fillColor(1, 0, 0);
			}
			beginPath();
			circle(curve[i], 10.f);
			fill();
		}

		
		for (std::size_t i = 0; i < offset.size(); ++i) {
			std::size_t mod = i % 4;
			if (mod == 0 || mod == 3) {
				fillColor(0.4, 1.0, 0.4);
			}
			else {
				fillColor(0.4, 1.0, 1.0);
			}
			beginPath();
			circle(offset[i], 4.f);
			fill();
		}
		beginPath();
		circle(offset.back(), 4.f);
		fill();
	}

	
};

int main() {
	Offset engine;
	return engine.run();
}