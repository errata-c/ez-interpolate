#pragma once
#include <Test.hpp>

#include <array>
#include <ez/geo/BezierFunctions.hpp>
#include "nanovg.h"

class CTangent : public Test {
public:
	CTangent(NVGcontext* _ctx)
		: ctx(_ctx)
	{}

	void draw(glm::vec2 size) override {
		std::array<glm::vec2, 4> p{
			glm::vec2{0.2, 0.8},
			glm::vec2{0.8, 0.2},
			glm::vec2{0.2, 0.2},
			glm::vec2{0.8, 0.8},
		};

		nvgStrokeWidth(ctx, 3.f);
		
		nvgFillColor(ctx, nvgRGBf(0, 0, 1));
		for (auto& o : p) {
			o *= size;

			drawCircle(o, 5.f);
		}

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
		drawCubic(p);

		nvgStrokeColor(ctx, nvgRGBf(1, 0, 0));
		std::array<float, 4> interp{ 0.2f, 0.4f, 0.6f, 0.8f };
		for (float val : interp) {
			glm::vec2 loc = ez::Bezier::interpolate(p[0], p[1], p[2], p[3], val);
			glm::vec2 tan = ez::Bezier::tangent(p[0], p[1], p[2], p[3], val);

			tan *= 50.0f;

			nvgBeginPath(ctx);
			nvgMoveTo(ctx, loc.x, loc.y);
			loc += tan;
			nvgLineTo(ctx, loc.x, loc.y);
			nvgStroke(ctx);
		}
	}
	const std::string& getName() const override {
		static std::string name = "CTangent";
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