#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

class Context;
class DragTangent : public ez::Operator<Context> {
public:
	using Result = ez::OpResult;

	DragTangent()
		: Operator("drag_tangent")
	{}

	bool poll(const Context & context) const override {
		return context.hasPointSelect() && context.hasControlSelect() && context.hasInputSelect();
	}

	Result execute(Context& context) override {
		return Result::PassThrough;
	}

	Result invoke(const ez::InputState& state, Context& context) override {
		if (state.type == ez::InputEventType::MousePress && state.mouse.justPressed == ez::Mouse::Left) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			std::ptrdiff_t inputIndex = context.getInputSelect();
			ez::PointCloud<glm::vec2>& reduce = context.getReduction();

			tanStart = reduce.getKnot(pointIndex, inputIndex).tangent;
			mouseStart = mpos;

			return Result::Modal;
		}
		else {
			return Result::PassThrough;
		}
	}

	Result modal(const ez::InputState& state, Context& context) override {
		if (state.mouse.buttons.isReleased(ez::Mouse::Left)) {
			return Result::Finished;
		}
		else if (state.type == ez::InputEventType::MouseMove) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			std::ptrdiff_t inputIndex = context.getInputSelect();
			ez::PointCloud<glm::vec2>& reduce = context.getReduction();

			reduce.getKnot(pointIndex, inputIndex).tangent = (mpos - mouseStart) + tanStart;
			return Result::Modal;
		}
		else {
			return Result::PassThrough;
		}
	}

	bool cancel(Context& context) override {
		return true;
	}
private:
	glm::vec2 tanStart, mouseStart;
};