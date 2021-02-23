#pragma once

class Context;
class GUI {
public:
	GUI(Context& ctx);

	void draw();
private:
	Context& context;
};