#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

class Context;
class DragPoint : public ez::Operator<Context> {
public:
	using Result = ez::OpResult;

	DragPoint()
		: Operator("drag_point")
	{}

	bool poll(const Context& context) const override {
		return context.hasPointSelect() && context.hasControlSelect();
	}

	Result execute(Context& context) override {
		return Result::PassThrough;
	}

	Result invoke(const ez::InputState& state, Context& context) override {
		if (state.type == ez::InputEventType::MousePress && state.mouse.justPressed == ez::Mouse::Left) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			ez::PointCloud<glm::vec2>& reduce = context.getReduction();
			
			start = reduce.getPoint(pointIndex);
			offset = start - mpos;

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
		else if(state.type == ez::InputEventType::MouseMove) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			ez::PointCloud<glm::vec2>& reduce = context.getReduction();

			reduce.setPoint(pointIndex, mpos + offset);
			return Result::Modal;
		}
		else {
			return Result::PassThrough;
		}
	}

	bool cancel(Context & context) override {
		return true;
	}
private:

	glm::vec2 start, offset;
};