#include "engine.hpp"

#include <glm/geometric.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

#include <ez/bezier/Bezier.hpp>

class length : public Engine {
public:
	glm::vec2 interpolate(float t) {
		return ez::bezier::interpolate(curve[0], curve[1], curve[2], curve[3], t);
	}

	float simpleLength() {
		int count = 1000;
		float t = 0.f;
		float delta = 1.f / (count - 1);

		float total = 0.f;

		glm::vec2 prev = interpolate(t);
		for (int i = 0; i < count; ++i, t += delta) {
			glm::vec2 pos = interpolate(t);
			total += glm::length(pos - prev);
			prev = pos;
		}

		return total;
	}

	length()
		: Engine("Bezier Length")
		, index(-1)
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
		float len0 = simpleLength();
		float len1 = ez::bezier::length(curve[0], curve[1], curve[2], curve[3]);

		ImGui::Begin("Length");

		ImGui::Text("Length calculated from line segments:");
		ImGui::Text("%.8f", len0);

		ImGui::Separator();

		ImGui::Text("Length calculated using circle arc approximation:");
		ImGui::Text("%.8f", len1);

		ImGui::Separator();

		ImGui::Text("Difference of %.8f", len0 - len1);

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
	}

	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve;
};


int main() {
	length engine;
	return engine.run();
}