#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

class Context;
class DragTangent : public ez::Operator {
public:
	using Result = ez::Operator::Result;

	DragTangent(Context& ctx)
		: ez::Operator("drag_tangent")
		, context(ctx)
	{}

	bool poll() const override {
		return context.hasPointSelect() && context.hasControlSelect() && context.hasInputSelect();
	}

	Result execute() override {
		return Result::PASS_THROUGH;
	}

	Result invoke(const ez::InputState& state) override {
		if (state.type == ez::InputEventType::MousePress && state.mouseJustPressed == ez::Mouse::Left) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			std::ptrdiff_t inputIndex = context.getInputSelect();
			ez::PointCloud<float, 2>& reduce = context.getReduction();

			tanStart = reduce.getKnot(pointIndex, inputIndex).tangent;
			mouseStart = mpos;

			return Result::RUNNING_MODAL;
		}
		else {
			return Result::PASS_THROUGH;
		}
	}

	Result modal(const ez::InputState& state) override {
		if (state.mouse.isReleased(ez::Mouse::Left)) {
			return Result::FINISHED;
		}
		else if (state.type == ez::InputEventType::MouseMove) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			std::ptrdiff_t inputIndex = context.getInputSelect();
			ez::PointCloud<float, 2>& reduce = context.getReduction();

			reduce.getKnot(pointIndex, inputIndex).tangent = (mpos - mouseStart) + tanStart;
			return Result::RUNNING_MODAL;
		}
		else {
			return Result::PASS_THROUGH;
		}
	}

	bool cancel() {
		return true;
	}
private:
	Context& context;

	glm::vec2 tanStart, mouseStart;
};