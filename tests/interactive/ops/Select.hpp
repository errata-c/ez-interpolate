#pragma once
#include <ez/input/Operator.hpp>
#include <glm/geometric.hpp>
#include "../context.hpp"
#include <fmt/printf.h>

class Context;
class Select : public ez::Operator<Context> {
public:
	using Result = ez::OpResult;

	Select()
		: Operator("select")
	{}

	bool poll(const Context & context) const override {
		return context.hasReduction();
	}

	Result execute(Context& context) override {
		return Result::PassThrough;
	}

	Result invoke(const ez::InputState& state, Context& context) override {
		if (state.type == ez::InputEventType::MousePress && state.mouse.justPressed == ez::Mouse::Left) {
			

			glm::vec2 mpos = state.mouse.position;
			ez::PointCloud<glm::vec2>& reduce = context.getReduction();

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

				return Result::Finished;
			}
			else {
				return Result::PassThrough;
			}
		}
		else {
			return Result::PassThrough;
		}
	}
};