#include "ez/geo/Bezier.h"
#include <cassert>
#include <glm/geometric.hpp>
#include <ez/math/MathConstants.h>
#include <ez/math/Polynomial.h>


namespace ez {
	namespace Bezier {
		std::array<glm::vec2, 3> quadraticCoefficients(const std::array<glm::vec2, 3> & points) {
			std::array<glm::vec2, 3> ret;
			ret[0] = (points[0] - 2.f * points[1] + points[2]);
			ret[1] = 2.f * points[1] - 2.f * points[0];
			ret[2] = points[0];
			return ret;
		}
		std::array<glm::vec2, 3> quadraticCoefficients(const glm::vec2 points[3]) {
			std::array<glm::vec2, 3> ret;
			ret[0] = (points[0] - 2.f * points[1] + points[2]);
			ret[1] = 2.f * points[1] - 2.f * points[0];
			ret[2] = points[0];
			return ret;
		}

		std::array<glm::vec2, 4> cubicCoefficients(const std::array<glm::vec2, 4> & points) {
			std::array<glm::vec2, 4> ret;
			ret[0] = points[3] - 3.f * points[2] + 3.f * points[1] - points[0];
			ret[1] = 3.f * points[2] - 6.f * points[1] + 3.f * points[0];
			ret[2] = 3.f * points[1] - 3.f * points[0];
			ret[3] = points[0];
			return ret;
		}

		std::array<glm::vec2, 4> cubicCoefficients(const glm::vec2 points[4]) {
			std::array<glm::vec2, 4> ret;
			ret[0] = points[3] - 3.f * points[2] + 3.f * points[1] - points[0];
			ret[1] = 3.f * points[2] - 6.f * points[1] + 3.f * points[0];
			ret[2] = 3.f * points[1] - 3.f * points[0];
			ret[3] = points[0];
			return ret;
		}

		std::array<glm::vec2, 2> quadraticDerivativeCoefficients(const std::array<glm::vec2, 3> & points) {
			std::array<glm::vec2, 2> ret;
			ret[0] = (points[0] - 2.f * points[1] + points[2]) * 2.f;
			ret[1] = (2.f * points[1] - 2.f * points[0]);
			return ret;
		}

		std::array<glm::vec2, 3> cubicDerivativeCoefficients(const std::array<glm::vec2, 4> & points) {
			std::array<glm::vec2, 3> ret;
			ret[0] = (points[3] - 3.f * points[2] + 3.f * points[1] - points[0]) * 3.f;
			ret[1] = (3.f * points[2] - 6.f * points[1] + 3.f * points[0]) * 2.f;
			ret[2] = (3.f * points[1] - 3.f * points[0]);
			return ret;
		}

		std::array<glm::vec2, 2> quadraticDerivative(const std::array<glm::vec2, 3> & points) {
			std::array<glm::vec2, 2> ret;
			// Distribution helps with precision issues.
			ret[0] = 2.f * points[1] - 2.f * points[0];
			ret[1] = 2.f * points[2] - 2.f * points[1];
			return ret;
		}

		std::array<glm::vec2, 3> cubicDerivative(const std::array<glm::vec2, 4> & points) {
			std::array<glm::vec2, 3> ret;
			ret[0] = 3.f * points[1] - 3.f * points[0];
			ret[1] = 3.f * points[2] - 3.f * points[1];
			ret[2] = 3.f * points[3] - 3.f * points[2];
			return ret;
		}

		AABB2d<float> quadraticBounds(const std::array<glm::vec2, 3> & points) {
			// Calculate extrema, build bounds box from it.
			auto derivCoeff = quadraticDerivativeCoefficients(points);

			float xRoot, yRoot;
			int xCount = ez::Polynomial::solveLinear(derivCoeff[0].x, derivCoeff[1].x, xRoot);
			int yCount = ez::Polynomial::solveLinear(derivCoeff[0].y, derivCoeff[1].y, yRoot);

			// The start and end points are both valid points on the curve, so start with the bound box they create.
			AABB2d<float> ret(points[0], points[2]);

			// Then just merge with the extrema, if they are within the domain.
			if (xCount && xRoot >= 0.f && xRoot <= 1.f) {
				ret.growToInclude(interpolate(points, xRoot));
			}
			if (yCount && yRoot >= 0.f && yRoot <= 1.f) {
				ret.growToInclude(interpolate(points, yRoot));
			}

			return ret;
		}

		AABB2d<float> cubicBounds(const std::array<glm::vec2, 4> & points) {
			auto derivCoeff = cubicDerivativeCoefficients(points);

			float xRoots[2];
			float yRoots[2];
			int xCount = ez::Polynomial::solveQuadratic(derivCoeff[0].x, derivCoeff[1].x, derivCoeff[2].x, xRoots, xRoots + 2);
			int yCount = ez::Polynomial::solveQuadratic(derivCoeff[0].y, derivCoeff[1].y, derivCoeff[2].y, yRoots, yRoots + 2);

			AABB2d<float> ret(points[0], points[3]);

			for (int i = 0; i < xCount; ++i) {
				if (xRoots[i] >= 0.f && xRoots[i] <= 1.f) {
					ret.growToInclude(interpolate(points, xRoots[i]));
				}
			}
			for (int i = 0; i < yCount; ++i) {
				if (yRoots[i] >= 0.f && yRoots[i] <= 1.f) {
					ret.growToInclude(interpolate(points, yRoots[i]));
				}
			}

			return ret;
		}

	}
};