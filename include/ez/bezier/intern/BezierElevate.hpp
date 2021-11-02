#pragma once
#include <cstddef>
#include <cassert>
#include <limits>
#include <ez/meta.hpp>
#include <glm/geometric.hpp>

namespace ez::bezier {
	// Curve elevation follows a specific pattern
	// 
	// D = degree of original curve
	// K = D + 1
	// i = index of control in the original curve
	// 
	// For each new control point being calculated
	//    ncontrol[i] = ( i * control[i-1] + (K - i) * control[i] ) / K


	// Elevate a linear bezier curve to a quadratic bezier curve
	template<typename vec_t, typename ouput_iter>
	void elevate(const vec_t & p0, const vec_t & p1, ouput_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::elevate requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::elevate requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::elevate requires an output iterator type as the last argument!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::elevate requires that the output iterator accept glm vector types as output!");

		*output++ = p0;
		*output++ = (p0 + p1) / T(2);
		*output++ = p1;
	}

	// Elevate a quadratic bezier curve to a cubic bezier curve
	template<typename vec_t, typename ouput_iter>
	void elevate(const vec_t& p0, const vec_t& p1, const vec_t& p2, ouput_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::elevate requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::elevate requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::elevate requires an output iterator type as the last argument!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::elevate requires that the output iterator accept glm vector types as output!");

		constexpr T a = T(1) / T(3), b = T(2) / T(3);

		*output++ = p0;
		*output++ = p0 * a + p1 * b;
		*output++ = p1 * b + p2 * a;
		*output++ = p2;
	}

	// Elevate a quadratic bezier curve to a cubic bezier curve
	template<typename vec_t, typename ouput_iter>
	void elevate(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, ouput_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::elevate requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::elevate requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::elevate requires an output iterator type as the last argument!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::elevate requires that the output iterator accept glm vector types as output!");

		constexpr T a = T(1) / T(4), b = T(2) / T(4), c = T(3) / T(4);

		*output++ = p0;
		*output++ = p0 * a + p1 * c;
		*output++ = p1 * b + p2 * b;
		*output++ = p2 * c + p3 * a;
		*output++ = p3;
	}

	// Elevate an arbitrary bezier curve
	template<typename input_iter, typename ouput_iter>
	void elevateRange(input_iter first, input_iter last, ouput_iter output) {
		static_assert(ez::is_random_iterator_v<input_iter>, "ez::bezier::elevateRange requires a random access iterator for input!");
		using vec_t = ez::iterator_value_t<input_iter>;
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::elevateRange requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::elevateRange requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::elevateRange requires an output iterator type as the last argument!");
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::elevateRange requires that the output iterator accept glm vector types as output!");

		std::ptrdiff_t D = last - first;
		if (D < 1) {
			return;
		}

		T K = static_cast<T>(D + 1);
		assert(static_cast<std::ptrdiff_t>(K) == (D + 1)); // Just check that the conversion to floating point was lossless.
		T i(1);
		
		vec_t prev = *first++;
		*output++ = prev;

		vec_t current = prev;
		while (first != last) {
			current = *first++;

			vec_t control = prev * i + current * (K - i);
			*output++ = control;

			i += T(1);
			prev = current;
		}

		*output++ = current;
	}
}