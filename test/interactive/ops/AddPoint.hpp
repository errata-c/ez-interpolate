#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

class Context;
class AddPoint: public ez::Operator {
public:
	using Result = ez::Operator::Result;

	AddPoint(Context& ctx)
		: ez::Operator("add_point")
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
			reduce.append(mpos);
			std::ptrdiff_t index = reduce.numPoints() - 1;

			std::vector<float> & inputs = context.getInputs();
			for (std::ptrdiff_t i = 0; i < inputs.size(); ++i) {
				auto& knot = reduce.getKnot(index, i);
				knot.position = inputs[i];
			}

			return Result::FINISHED;
		}
		else {
			return Result::PASS_THROUGH;
		}
	}
private:
	Context& context;
};