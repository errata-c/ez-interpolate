#include "engine.hpp"

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class Split : public Engine {
public:
	float sp0, sp1;
	int state;
	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve, left, right;

	Split()
		: Engine("Bezier Offsets")
		, index(-1)
		, sp0(0.5f)
		, sp1(0.75f)
		, state(0)
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
			ImGui::RadioButton("Single Split", &state, 0);
			if (ImGui::RadioButton("Segment Split", &state, 1)) {
				sp1 = std::max(sp0, sp1);
			}

			if (state == 0) {
				ImGui::DragFloat("Split Point", &sp0, 0.005f, -0.2f, 1.2f, "%.3f", 1.f);
			}
			else {
				ImGui::DragFloat("Split Point 0", &sp0, 0.005f, -0.2f, 1.2f, "%.3f", 1.f);
				ImGui::DragFloat("Split Point 1", &sp1, 0.005f, -0.2f, 1.2f, "%.3f", 1.f);
				if (sp0 > sp1) {
					std::swap(sp0, sp1);
				}
			}
		}
		ImGui::End();
	}

	void render() override {
		strokeWidth(3);

		if (state == 0) {
			std::array<glm::vec2, 7> gen;
			ez::bezier::split(curve[0], curve[1], curve[2], curve[3], sp0, gen.begin());
			//ez::bezier::leftSplit(curve[0], curve[1], curve[2], curve[3], sp0, left.begin());
			//ez::bezier::rightSplit(curve[0], curve[1], curve[2], curve[3], sp0, right.begin());

			strokeColor(0.8, 0., 0);
			beginPath();
			moveTo(gen[0]);
			pathTo(gen[1], gen[2], gen[3]);
			stroke();

			strokeColor(0, 0.1, 0.8);
			beginPath();
			moveTo(gen[3]);
			pathTo(gen[4], gen[5], gen[6]);
			stroke();

			fillColor(0, 1, 0);
			beginPath();
			circle(gen[3], 5.f);
			fill();
		}
		else {
			std::array<glm::vec2, 4> gen;
			ez::bezier::segment(curve[0], curve[1], curve[2], curve[3], sp0, sp1, gen.begin());

			strokeColor(0.8, 0., 0);
			beginPath();
			moveTo(gen[0]);
			pathTo(gen[1], gen[2], gen[3]);
			stroke();

			glm::vec2 leftPoint = ez::bezier::interpolateStatic<4>(curve.begin(), sp0);
			glm::vec2 rightPoint = ez::bezier::interpolateStatic<4>(curve.begin(), sp1);

			fillColor(0, 1, 0);
			beginPath();
			circle(leftPoint, 5.f);
			fill();

			beginPath();
			circle(rightPoint, 5.f);
			fill();
		}

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