#pragma once
#include <string>
#include <glm/vec2.hpp>
#include "nanovg.h"

class Test {
public:
	virtual void draw(glm::vec2 size) = 0;
	virtual const std::string& getName() const = 0;
};