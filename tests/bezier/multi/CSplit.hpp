#pragma once
#include <Test.hpp>
#include <array>
#include <ez/geo/BezierFunctions.hpp>
#include "nanovg.h"

class CSplit: public Test {
public:
	CSplit(NVGcontext* _ctx)
		: ctx(_ctx)
	{}

	void draw(glm::vec2 size) override {
		std::array<glm::vec2, 4> p{
			glm::vec2{0.2, 0.6},
			glm::vec2{0.4, 0.4},
			glm::vec2{0.6, 0.4},
			glm::vec2{0.8, 0.6},
		};

		for (auto& o : p) {
			o *= size;
		}

		nvgStrokeWidth(ctx, 3.f);

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
		drawCubic(p);

		std::array<glm::vec2, 4> left, right;

		ez::Bezier::leftSplit(p[0], p[1], p[2], p[3], 0.5f, left.begin());
		ez::Bezier::rightSplit(p[0], p[1], p[2], p[3], 0.5f, right.begin());

		for (auto& o : left) {
			o.y += 0.05f * size.y;
		}
		for (auto& o : right) {
			o.y -= 0.05f * size.y;
		}

		nvgStrokeColor(ctx, nvgRGBf(1, 0, 0));
		drawCubic(left);

		nvgStrokeColor(ctx, nvgRGBf(0, 1, 0));
		drawCubic(right);
	}
	const std::string& getName() const override {
		static std::string name = "CSplit";
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
};
