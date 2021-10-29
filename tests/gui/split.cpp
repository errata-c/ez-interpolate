#include "engine.hpp"

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class Split : public Engine {
public:
	float splitPoint;
	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve, left, right;

	Split()
		: Engine("Bezier Offsets")
		, index(-1)
		, splitPoint(0.5f)
	{
		curve[0] = glm::vec2(100, 500);
		curve[1] = glm::vec2(100, 100);
		curve[2] = glm::vec2(700, 100);
		curve[3] = glm::vec2(700, 500);
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
		if(ImGui::Begin("Edit")) {
			//ImGui::RadioButton("Taper", &mode, 1);
			//ImGui::RadioButton("Normal", &mode, 2);

			ImGui::DragFloat("Split Point", &splitPoint, 0.005f, 0.f, 1.f, "%.3f", 1.f);
		}
		ImGui::End();
	}

	void render() override {
		strokeWidth(3);

		ez::bezier::leftSplit(curve[0], curve[1], curve[2], curve[3], splitPoint, left.begin());
		ez::bezier::rightSplit(curve[0], curve[1], curve[2], curve[3], splitPoint, right.begin());

		strokeColor(0.8, 0., 0);
		beginPath();
		moveTo(left[0]);
		pathTo(left[1], left[2], left[3]);
		stroke();

		strokeColor(0, 0.1, 0.8);
		beginPath();
		moveTo(right[0]);
		pathTo(right[1], right[2], right[3]);
		stroke();

		for (int i = 0; i < 4; ++i) {
			if (i == index) {
				fillColor(1, 0, 0);
			}
			else {
				fillColor(0, 0, 1);
			}
			beginPath();
			circle(curve[i], 10.f);
			fill();
		}
	}
};

int main() {
	Split engine;
	return engine.run();
}