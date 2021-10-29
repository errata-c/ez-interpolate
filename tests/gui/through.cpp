#include "engine.hpp"

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class Through : public Engine {
public:
	std::ptrdiff_t index;
	std::array<glm::vec2, 4> curve, fitted;

	Through()
		: Engine("Bezier through points")
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
		
	}

	void render() override {
		strokeWidth(3);

		fitted[0] = curve[0];
		fitted[3] = curve[3];
		ez::bezier::curveThrough(curve[0], curve[1], curve[2], curve[3], &fitted[1]);

		strokeColor(0, 0, 0);
		beginPath();
		moveTo(fitted[0]);
		pathTo(fitted[1], fitted[2], fitted[3]);
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
	Through engine;
	return engine.run();
}