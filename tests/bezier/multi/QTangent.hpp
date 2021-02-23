#pragma once
#include <Test.hpp>

#include <array>
#include <ez/geo/BezierFunctions.hpp>

class QTangent : public Test {
public:
	QTangent(NVGcontext* _ctx)
		: ctx(_ctx)
	{}

	void draw(glm::vec2 size) override {
		std::array<glm::vec2, 3> p{
			glm::vec2{0.2, 0.6},
			glm::vec2{0.5, 0.4},
			glm::vec2{0.8, 0.6},
		};

		nvgStrokeWidth(ctx, 3.f);

		nvgFillColor(ctx, nvgRGBf(0, 0, 1));
		for (auto& o : p) {
			o *= size;

			drawCircle(o, 5.f);
		}

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
		drawQuadratic(p);

		nvgStrokeColor(ctx, nvgRGBf(1, 0, 0));
		std::array<float, 4> interp{ 0.2f, 0.4f, 0.6f, 0.8f};
		for (float val : interp) {
			glm::vec2 loc = ez::Bezier::interpolate(p[0], p[1], p[2], val);
			glm::vec2 tan = ez::Bezier::tangent(p[0], p[1], p[2], val);

			tan *= 50.0f;

			nvgBeginPath(ctx);
			nvgMoveTo(ctx, loc.x, loc.y);
			loc += tan;
			nvgLineTo(ctx, loc.x, loc.y);
			nvgStroke(ctx);
		}
	}
	const std::string& getName() const override {
		static std::string name = "QTangent";
		return name;
	}
private:
	NVGcontext* ctx;

	void drawQuadratic(const std::array<glm::vec2, 3>& p) {
		glm::vec2
			m0 = p[0] * (1.f / 3.f) + p[1] * (2.f / 3.f),
			m1 = p[2] * (1.f / 3.f) + p[1] * (2.f / 3.f);

		nvgBeginPath(ctx);
		nvgMoveTo(ctx, p[0].x, p[0].y);
		nvgBezierTo(ctx, m0.x, m0.y, m1.x, m1.y, p[2].x, p[2].y);
		nvgStroke(ctx);
	}
	void drawCircle(glm::vec2 pos, float radius) {
		nvgBeginPath(ctx);
		nvgArc(ctx, pos.x, pos.y, radius, 0.f, ez::tau<float>(), NVG_CCW);
		nvgFill(ctx);
	}
};