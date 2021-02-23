#pragma once
#include <Test.hpp>

#include <array>
#include <ez/geo/BezierFunctions.hpp>
#include "nanovg.h"

class CThrough : public Test {
public:
	CThrough(NVGcontext* _ctx)
		: ctx(_ctx)
	{}

	void draw(glm::vec2 size) override {
		std::array<glm::vec2, 4> p{
			glm::vec2{0.2, 0.6},
			glm::vec2{0.4, 0.4},
			glm::vec2{0.6, 0.4},
			glm::vec2{0.8, 0.6},
		};

		nvgStrokeWidth(ctx, 3.f);

		nvgFillColor(ctx, nvgRGBf(0, 0, 1));
		for (auto& o : p) {
			o *= size;

			drawCircle(o, 5.f);
		}

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
		ez::Bezier::curveThrough(p[0], p[1], p[2], p[3], p.begin() + 1);

		drawCubic(p);
	}
	const std::string& getName() const override {
		static std::string name = "CThrough";
		return name;
	}
private:
	NVGcontext* ctx;

	void drawCubic(const std::array<glm::vec2, 4>& p) {
		nvgBeginPath(ctx);
		nvgMoveTo(ctx, p[0].x, p[0].y);
		nvgBezierTo(ctx, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
		nvgStroke(ctx);
	}
	void drawCircle(glm::vec2 pos, float radius) {
		nvgBeginPath(ctx);
		nvgArc(ctx, pos.x, pos.y, radius, 0.f, ez::tau<float>(), NVG_CCW);
		nvgFill(ctx);
	}
};