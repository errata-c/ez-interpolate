#include "engine.hpp"

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class Offset : public Engine {
public:
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
	}

	void handleEvent(const ez::InputEvent& ev) override {
		if (index == -1) {
			if (ev.type == ez::InputEventType::MousePress && ev.mouse.buttons == ez::Mouse::Left) {
				glm::vec2 mpos = ev.mouse.position;

				float dist2 = 8;
				std::ptrdiff_t select = -1;

				std::ptrdiff_t count = static_cast<std::ptrdiff_t>(4);
				for (std::ptrdiff_t i = 0; i < count; ++i) {
					glm::vec2 cpos = curve[i];
					float dist1 = glm::length(cpos - mpos);
					if (dist1 < dist2) {
						select = i;
						dist2 = dist1;
					}
				}

				index = select;
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
		ImGui::RadioButton("Normal", &mode, 2);

		ImGui::DragFloat4("Tapers", &taperValues[0], 1.f, 0.f, 64.f, "%.0f", 1.f);

		ImGui::End();
	}

	void render() override {
		strokeColor(0, 0, 0);
		fillColor(0.2, 0.2, 0.2);
		strokeWidth(3);

		beginPath();
		moveTo(curve[0]);
		pathTo(curve[1], curve[2], curve[3]);
		stroke();

		offset.clear();
		if (mode == 1) {
			ez::bezier::taperedPixelOffset(curve[0], curve[1], curve[2], curve[3], taperValues, std::back_inserter(offset));
		}
		else {
			ez::bezier::pixelOffset(curve[0], curve[1], curve[2], curve[3], 32.f, std::back_inserter(offset));
		}

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

		fillColor(0.4, 0.4, 0.4);
		for (std::size_t i = 0; i < offset.size(); i += 4) {
			beginPath();
			circle(offset[i], 10.f);
			fill();
		}
		beginPath();
		circle(offset.back(), 10.f);
		fill();
	}

	glm::vec4 taperValues;

	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve;
	std::vector<glm::vec2> offset;

	int mode;
};

int main() {
	Offset engine;
	return engine.run();
}