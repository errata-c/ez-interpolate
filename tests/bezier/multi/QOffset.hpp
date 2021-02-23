#pragma once
#include <Test.hpp>

#include <vector>
#include <array>
#include <chrono>
#include <ez/geo/BezierFunctions.hpp>
#include <ez/math/MathConstants.hpp>

class QOffset : public Test {
public:
	QOffset(NVGcontext* _ctx)
		: ctx(_ctx)
	{
		prev = std::chrono::high_resolution_clock().now();
	}

	void draw(glm::vec2 size) override {
		std::array<glm::vec2, 3> p{
			glm::vec2{0.2, 0.6},
			glm::vec2{0.5, 0.4},
			glm::vec2{0.8, 0.6},
		};

		for (auto& o : p) {
			o *= size;
		}

		nvgStrokeWidth(ctx, 3.f);

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
		drawQuadratic(p);

		static float theta = 0.0f;

		auto ctime = std::chrono::high_resolution_clock().now();
		float delTime = std::chrono::duration_cast<std::chrono::duration<float>>(ctime - prev).count();
		prev = ctime;

		theta += delTime * ez::tau<float>();
		float delta = size.y * 0.1f * std::cos(theta);
		if (theta > ez::tau<float>()) {
			theta = 0.0f;
		}

		std::vector<glm::vec2> off;
		ez::Bezier::pixelOffset(p[0], p[1], p[2], delta, std::back_inserter(off));
		
		nvgStrokeColor(ctx, nvgRGBf(0, 0, 1));
		nvgFillColor(ctx, nvgRGBf(1, 0, 0));
		for (std::size_t i = 0; i < off.size(); i += 3) {
			std::array<glm::vec2, 3> curve{
				off[i],
				off[i + 1],
				off[i + 2]
			};
			drawQuadratic(curve);
		}

		for (std::size_t i = 0; i < off.size(); i += 3) {
			drawPoint(off[i], 3);
		}
		if (off.size()) {
			drawPoint(off.back(), 3);
		}
	}
	const std::string& getName() const override {
		static std::string name = "QOffset";
		return name;
	}
private:
	NVGcontext* ctx;
	std::chrono::high_resolution_clock::time_point prev;

	void drawQuadratic(const std::array<glm::vec2, 3>& p) {
		glm::vec2
			m0 = p[0] * (1.f / 3.f) + p[1] * (2.f / 3.f),
			m1 = p[2] * (1.f / 3.f) + p[1] * (2.f / 3.f);

		nvgBeginPath(ctx);
		nvgMoveTo(ctx, p[0].x, p[0].y);
		nvgBezierTo(ctx, m0.x, m0.y, m1.x, m1.y, p[2].x, p[2].y);
		nvgStroke(ctx);
	}

	void drawPoint(glm::vec2 pos, float radius) {
		nvgBeginPath(ctx);
		nvgArc(ctx, pos.x, pos.y, radius, 0.f, ez::tau<float>(), NVG_CCW);
		nvgFill(ctx);
	}
};