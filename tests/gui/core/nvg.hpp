#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct NVGcontext * nvgCreate();
void nvgDelete(struct NVGcontext* vg);