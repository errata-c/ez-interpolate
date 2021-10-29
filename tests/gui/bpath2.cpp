#include "engine.hpp"

#include <glm/geometric.hpp>

#include "BPath2.hpp"
#include <ez/bezier/BPath.hpp>

#include <gl/glew.h>
#include "nanovg.h"
#include "imgui.h"

class bpath : public Engine {
public:
	using Path = ez::ClosedPath;

	bpath()
		: Engine("BPath test")
		, index(-1)
		, closed(true)
	{
		for (float t = 0, end = ez::tau<float>() *(11.0 / 12.0); t <= end; t += ez::tau<float>() / 6.f) {
			path.append({ -std::cos(-t)*0.4 +0.5, std::sin(-t)*0.4 +0.5 });
		}
		//path.append({ false, { 0.2, 0.8 } });
		//path.append({ false, { 0.2, 0.2 }});
		//path.append({ false, { 0.4, 0.1 }});

		//path.append({ false, { 0.8, 0.2 }});
		//path.append({ false, { 0.8, 0.8 }});
		//path.append({ false, { 0.4, 0.9 }});
	}
	~bpath() {}

	void handleEvent(const ez::InputEvent& ev) override {
		if (index == -1) {
			if (ev.type == ez::InputEventType::MousePress && ev.mouse.buttons == ez::Mouse::Left) {
				glm::vec2 mpos = ev.mouse.position;
				glm::vec2 frame = getFrameSize();

				float dist2 = 8;
				std::ptrdiff_t select = -1;

				std::ptrdiff_t count = static_cast<std::ptrdiff_t>(path.size());
				for (std::ptrdiff_t i = 0; i < count; ++i) {
					glm::vec2 cpos = path[i];
					float dist1 = glm::length(cpos * frame - mpos);
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
				glm::vec2 frame = getFrameSize();
				mpos /= frame;

				path[index] = mpos;
			}
		}
	}
	void gui() override {
		if (ImGui::Begin("PATH AAAAAA")) {
			ImGui::Text("Points == %d", path.numPoints());
			ImGui::Text("Segments == %d", path.numSegments());
		}
		ImGui::End();
	}
	void render() override {
		glm::vec2 size = getFrameSize();

		strokeWidth(6.f);
		
		for (std::ptrdiff_t i = 0; i < path.size(); ++i) {
			const glm::vec2 & point = path[i];

			if (i == index) {
				fillColor(1, 0, 0);
			}
			else {
				fillColor(0, 0, 1);
			}
			
			beginPath();
			circle(point * size, 10.f);
			fill();
		}

		std::size_t count = path.numSegments();
		for (std::size_t i = 0; i < count; ++i) {
			auto seg = path.segmentAt(i);
			for (auto& point : seg) {
				point = point * size;
			}

			switch (i % 3) {
			case 0:
				strokeColor(0.6, 0.1, 0.0);
				break;
			case 1:
				strokeColor(0.0, 0.2, 0.6);
				break;
			case 2:
				strokeColor(0.0, 0.6, 0.1);
				break;
			}

			beginPath();
			moveTo(seg[0]);
			pathTo(seg[1], seg[2], seg[3]);
			stroke();
		}
	}

	Path path;
	std::ptrdiff_t index;
	bool closed;
};

int main() {
	bpath engine;

	return engine.run();
}