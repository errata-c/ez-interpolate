#pragma once
#include <ez/meta.hpp>
#include <glm/geometric.hpp>
#include "BezierInterpolation.hpp"

namespace ez::bezier {
	// Writes the new middle control using the output iterator.
	// The final curve will have the same start and end points as the inputs.
	template<typename vec_t, typename Iter>
	void curveThrough(const vec_t& p0, const vec_t& p1, const vec_t& p2, Iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
		using T = vec_value_t<vec_t>;

		static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

		*output++ = (p1 - p0 * T(0.25) - p2 * T(0.25)) * T(2.0);
	}

	// Returns the two middle controls, assuming p0 and p3 are the start and end controls.
	// The final curve will have the same start and end points as the inputs.
	template<typename vec_t, typename Iter>
	void curveThrough(const vec_t& a, const vec_t& k0, const vec_t& k1, const vec_t& d, Iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
		using T = vec_value_t<vec_t>;

		static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

		vec_t b =
			T(54.0 / 18.0) * k0 +
			T(-27.0 / 18.0) * k1 +
			T(-15.0 / 18.0) * a +
			T(6.0 / 18.0) * d;

		vec_t c =
			T(27.0 / 6.0) * k0 +
			T(-8.0 / 6.0) * a +
			T(-12.0 / 6.0) * b +
			T(-1.0 / 6.0) * d;

		*output++ = b;
		*output++ = c;
	}

	namespace intern {
		template<typename input_iter, typename output_iter>
		struct CurveThroughExpander {
			input_iter input;
			output_iter output;

			template<std::size_t N, typename ...Ts>
			void call(Ts&&... args) {
				if constexpr (N == 0) {
					return bezier::curveThrough(std::forward<Ts>(args)..., output);
				}
				else {
					return call<N - 1>(std::forward<Ts>(args)..., *input++);
				}
			}
		};
	}

	template<std::size_t N, typename input_iter, typename output_iter>
	void curveThroughStatic(input_iter input, output_iter output) {
		return intern::CurveThroughExpander<input_iter, output_iter>{input, output}.call<N>();
	}
};