#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

#include <fmt/core.h>

class Context;
class AddPoint: public ez::Operator<Context> {
public:
	using Result = ez::OpResult;

	AddPoint()
		: Operator("add_point")
	{}

	bool poll(const Context & context) const override {
		return context.hasReduction();
	}

	Result execute(Context& context) override {
		return Result::PassThrough;
	}

	Result invoke(const ez::InputState& state, Context & context) override {
		if (state.type == ez::InEv::MousePress && state.mouse.justPressed == ez::Mouse::Left) {

			glm::vec2 mpos = state.mouse.position;

			ez::PointCloud<glm::vec2>& reduce = context.getReduction();
			reduce.append(mpos);
			std::ptrdiff_t index = reduce.numPoints() - 1;

			std::vector<float> & inputs = context.getInputs();
			for (std::ptrdiff_t i = 0; i < inputs.size(); ++i) {
				auto& knot = reduce.getKnot(index, i);
				knot.position = inputs[i];
			}

			return Result::Finished;
		}
		else {
			return Result::PassThrough;
		}
	}
};