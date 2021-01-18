#pragma once
#include <ez/input/Operator.hpp>
#include "../context.hpp"

class Context;
class DragPoint : public ez::Operator {
public:
	using Result = ez::Operator::Result;

	DragPoint(Context& ctx)
		: ez::Operator("drag_point")
		, context(ctx)
	{}

	bool poll() const override {
		return context.hasPointSelect() && context.hasControlSelect();
	}

	Result execute() override {
		return Result::PASS_THROUGH;
	}

	Result invoke(const ez::InputState& state) override {
		if (state.type == ez::InputEventType::MousePress && state.mouseJustPressed == ez::Mouse::Left) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			ez::PointCloud<float, 2>& reduce = context.getReduction();
			
			start = reduce.getPoint(pointIndex);
			offset = start - mpos;

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
		else if(state.type == ez::InputEventType::MouseMove) {
			glm::vec2 mpos = state.mouse.position;
			std::ptrdiff_t pointIndex = context.getControlSelect();
			ez::PointCloud<float, 2>& reduce = context.getReduction();

			reduce.setPoint(pointIndex, mpos + offset);
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

	glm::vec2 start, offset;
};