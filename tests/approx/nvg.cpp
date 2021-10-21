#include <GL/glew.h>

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

struct NVGcontext* nvgCreate() {
	return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
}
void nvgDelete(struct NVGcontext* vg) {
	nvgDeleteGL3(vg);
}