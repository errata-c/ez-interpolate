#pragma once
#include <Test.hpp>

#include <vector>
#include <array>
#include <ez/geo/BezierFunctions.hpp>
#include <ez/math/MathConstants.hpp>
#include "nanovg.h"

class CFitting: public Test {
public:
	CFitting(NVGcontext * _ctx)
		: ctx(_ctx)
	{}

	void draw(glm::vec2 size) override {
		std::array<float, 7> tdata{
			1.0f / 8.0f,
			2.0f / 8.0f,
			3.0f / 8.0f,

			4.0f / 8.0f,
			5.0f / 8.0f,
			6.0f / 8.0f,
			7.0f / 8.0f
		};
		std::array<float, 7> vdata{
			0.1f,
			0.2f,
			0.25f,

			0.26f,
			0.27f,
			0.3f,
			0.5f,
		};

		std::array<float, 4> curve;

		ez::Bezier::fitCubic(vdata.begin(), vdata.begin() + 7, tdata.begin(), tdata.begin() + 7, curve.begin());

		nvgStrokeWidth(ctx, 3.f);

		nvgFillColor(ctx, nvgRGBf(0, 0, 0));

		for (int i = 0; i < tdata.size(); ++i) {
			float x = tdata[i] * size.x;
			float y = vdata[i] * size.y;
			nvgBeginPath(ctx);
			nvgArc(ctx, x, y, 4.f, 0.f, ez::tau<float>(), NVG_CCW);
			nvgFill(ctx);
		}

		nvgStrokeColor(ctx, nvgRGBf(0, 0, 1));

		nvgBeginPath(ctx);
		nvgMoveTo(ctx, 0, curve[0] * size.y);
		for (float t = 0.01f; t < 1.0f; t += 0.01f) {
			float val = ez::Bezier::interpolate(curve[0], curve[1], curve[2], curve[3], t);
			
			nvgLineTo(ctx, t * size.x, val * size.y);
		}
		nvgStroke(ctx);

		nvgFillColor(ctx, nvgRGBf(1, 0, 0));
		for (int i = 0; i < curve.size(); ++i) {
			nvgBeginPath(ctx);

			nvgArc(ctx, (i / 3.0f) * size.x, curve[i] * size.y, 4.f, 0.0f, ez::tau<float>(), NVG_CCW);
			
			nvgFill(ctx);
		}

		static bool once = true;
		if (once) {
			double error = 0.0;
			for (int i = 0; i < tdata.size(); ++i) {
				float t = tdata[i];
				float v = vdata[i];

				float est = ez::Bezier::interpolate(curve[0], curve[1], curve[2], curve[3], t);

				float tmp = (v - est);
				error += tmp * tmp;
			}
			fmt::print("The net error is: {}\n", error);

			once = false;
		}
	}
	const std::string& getName() const override {
		static std::string name = "CFitting";
		return name;
	}
private:
	NVGcontext* ctx;
};