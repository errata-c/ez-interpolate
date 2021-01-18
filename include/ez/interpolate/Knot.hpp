#pragma once
#include "intern/PointType.hpp"

namespace ez {
	template<typename T, int N>
	struct Knot {
		using real_t = T;
		using vec_t = typename ez::intern::Point<T, N>::type;

		Knot()
			: domain(1)
			, position(0)
			, tangent(0)
		{}

		// The input value along the interpolation where the path passes through the point this knot is attached to.
		real_t position;

		// The scale factor for the domain of this knot, ie the X scale.
		real_t domain;

		// The direction this knot interpolates.
		vec_t tangent;

		real_t calcCoeff(real_t t) const {
			t = (t - position) * domain;
			return domain * domain / (t * t + real_t(1));
		}
		real_t calcInterp(real_t t) const {
			return t - position;
		}
	};
};