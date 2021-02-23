#include <fmt/core.h>
#include <glm/vec2.hpp>
#include <array>

double w(double x, int i, int k, double arr[]) {
	double denom = arr[i+k] - arr[i];
	if (std::abs(denom) < 1e-5f) {
		return 0;
	}
	else {
		return (x - arr[i]) / denom;
	}
}
double B(double x, int i, int k, double arr[]) {
	if (k == 1) {
		if (arr[i] <= x && x <= arr[i+1]) {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return B(x, i, k - 1, arr) * w(x, i, k - 1, arr) + (1.0 - w(x, i + 1, k - 1, arr)) * B(x, i + 1, k - 1, arr);
	}
}

int main(int argc, char* argv[]) {
	std::array<double, 7> knots;
	knots[0] = 0;
	knots[1] = 0;
	knots[2] = 0.25;
	knots[3] = 0.5;
	knots[4] = 0.75;
	knots[5] = 1;
	knots[6] = 1;

	for (double t = 0; t < 1.0; t += 0.1) {
		double res = B(t, 0, 4, knots.data());
		fmt::print("B0_4({:.2f}) = {:.3f}\n", t, res);
	}

	fmt::print("\n\n");
	for (double t = 0; t < 1.0; t += 0.1) {
		double res = B(t, 1, 4, knots.data());
		fmt::print("B1_4({:.2f}) = {:.3f}\n", t, res);
	}

	fmt::print("\n\n");
	for (double t = 0; t < 1.0; t += 0.1) {
		double res = B(t, 2, 4, knots.data());
		fmt::print("B2_4({:.2f}) = {:.3f}\n", t, res);
	}
	return 0;
}