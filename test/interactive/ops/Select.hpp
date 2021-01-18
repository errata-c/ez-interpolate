#pragma once
#include <ez/input/Operator.hpp>
#include <glm/geometric.hpp>
#include "../context.hpp"
#include <fmt/printf.h>

class Context;
class Select : public ez::Operator {
public:
	using Result = ez::Operator::Result;

	Select(Context& ctx)
		: ez::Operator("select")
		, context(ctx)
	{}

	bool poll() const override {
		return context.hasReduction();
	}

	Result execute() override {
		return Result::PASS_THROUGH;
	}

	Result invoke(const ez::InputState& state) override {
		if (state.type == ez::InputEventType::MousePress && state.mouseJustPressed == ez::Mouse::Button1) {
			glm::vec2 mpos = state.mouse.position;
			ez::PointCloud<float, 2>& reduce = context.getReduction();

			std::ptrdiff_t select = -1;
			float dist1 = 5.f;

			for (std::ptrdiff_t i = 0; i < reduce.numPoints(); ++i) {
				glm::vec2 cpos = reduce.getPoint(i);

				float dist2 = glm::length(cpos - mpos);
				if (dist2 < dist1) {
					select = i;
					dist1 = dist2;
				}
			}

			if (select != -1) {
				context.setControlSelect(select);
				
				return Result::FINISHED;
			}
			else {
				return Result::PASS_THROUGH;
			}
		}
		else {
			return Result::PASS_THROUGH;
		}
	}
private:
	Context& context;
};