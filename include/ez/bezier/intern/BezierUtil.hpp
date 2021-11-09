#pragma once
#include <ez/meta.hpp>
#include <ez/math/poly.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm>

namespace glm {
	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	GLM_FUNC_DECL T normalize(T const& x) {
		return x / glm::length(x);
	}
};

namespace ez::bezier {
	template<typename vec_t, typename output_iter>
	void coefficients(const vec_t& p0, const vec_t& p1, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = -p0 + p1;
		*output++ = p0;
	}
	template<typename vec_t, typename output_iter>
	void coefficients(const vec_t& p0, const vec_t& p1, const vec_t& p2, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = p0 + T(-2) * p1 + p2;
		*output++ = T(-2) * p0 + T(2) * p1;
		*output++ = p0;
	}
	template<typename vec_t, typename output_iter>
	void coefficients(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = -p0 + T(3) * p1 - T(3) * p2 + p3;
		*output++ = T(3) * p0 - T(6) * p1 + T(3) * p2;
		*output++ = T(-3) * p0 + T(3) * p1;
		*output++ = p0;
	}

	template<typename vec_t, typename output_iter>
	void derivativeCoefficients(const vec_t& p0, const vec_t& p1, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = -p0 + p1;
	}
	template<typename vec_t, typename output_iter>
	void derivativeCoefficients(const vec_t& p0, const vec_t& p1, const vec_t& p2, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = T(2) * (p0 + T(-2) * p1 + p2);
		*output++ = (T(-2) * p0 + T(2) * p1);
	}
	template<typename vec_t, typename output_iter>
	void derivativeCoefficients(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::coefficients requires a glm vec type!");
		using T = ez::vec_value_t<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::coefficients requires floating point types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "The iterator passed in is not a proper output iterator!");
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		*output++ = T(3) *(-p0 + T(3) * p1 - T(3) * p2 + p3);
		*output++ = T(2) * (T(3) * p0 - T(6) * p1 + T(3) * p2);
		*output++ = T(-3) * p0 + T(3) * p1;
	}

	namespace intern {
		template<typename input_iter, typename output_iter>
		struct CoefficientsExpander {
			input_iter input;
			output_iter output;

			template<std::size_t N, typename ... Ts>
			void call(Ts&&... args) {
				if constexpr (N == 0) {
					coefficients(std::forward<Ts>(args)..., output);
				}
				else {
					call<N - 1>(std::forward<Ts>(args)..., *input++);
				}
			}
		};

		template<typename input_iter, typename output_iter>
		struct DerivativesCoefficientsExpander {
			input_iter input;
			output_iter output;

			template<std::size_t N, typename ... Ts>
			void call(Ts&&... args) {
				if constexpr (N == 0) {
					derivativeCoefficients(std::forward<Ts>(args)..., output);
				}
				else {
					call<N - 1>(std::forward<Ts>(args)..., *input++);
				}
			}
		};
	}

	template<std::size_t N, typename input_iter, typename output_iter>
	void coefficientsStatic(input_iter input, output_iter output) {
		intern::CoefficientsExpander<input_iter, output_iter>{input, output}.call<N>();
	}
	template<std::size_t N, typename input_iter, typename output_iter>
	void derivativeCoefficientsStatic(input_iter input, output_iter output) {
		intern::DerivativeCoefficientsExpander<N, input_iter, output_iter>{input, output}.call<N>();
	}


	template<typename vec_t, typename output_iter>
	int findExtrema(const vec_t& p0, const vec_t& p1, const vec_t & p2, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::findExtrema requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		constexpr std::size_t Dim = ez::vec_length_v<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::findExtrema requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::findExtrema requires an output iterator as last argument!");
		static_assert(ez::is_iterator_writable_v<output_iter, T>, "ez::bezier::findExtrema requires that the output iterator accept floating point values!");

		std::array<vec_t, 3> coeff;
		bezier::coefficients(p0, p1, p2, &coeff[0]);

		coeff[0] *= T(2);

		int count = 0;
		std::array<T, Dim> roots;
		for (int i = 0; i < Dim; ++i) {
			count += ez::poly::solveLinear(
				ez::value_ptr(coeff[0])[i],
				ez::value_ptr(coeff[1])[i],
				&roots[count]);
		}
		auto nend = std::remove_if(roots.begin(), roots.begin() + count, [](T val) {
			return val < T(0) || T(1) < val;
		});
		count = nend - roots.begin();

		for (int i = 0; i < count; ++i) {
			*output++ = roots[i];
		}
		return count;
	}

	template<typename vec_t, typename output_iter>
	int findExtrema(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, output_iter output) {
		static_assert(ez::is_vec_v<vec_t>, "ez::bezier::findExtrema requires glm vector types!");
		using T = ez::vec_value_t<vec_t>;
		constexpr std::size_t Dim = ez::vec_length_v<vec_t>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::findExtrema requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::findExtrema requires an output iterator as last argument!");
		static_assert(ez::is_iterator_writable_v<output_iter, T>, "ez::bezier::findExtrema requires that the output iterator accept floating point values!");

		std::array<vec_t, 4> coeff;
		bezier::coefficients(p0, p1, p2, p3, &coeff[0]);

		coeff[0] *= T(3);
		coeff[1] *= T(2);

		int count = 0;
		std::array<T, 2 * Dim> roots;
		for (int i = 0; i < Dim; ++i) {
			count += ez::poly::solveQuadratic(
				ez::value_ptr(coeff[0])[i], 
				ez::value_ptr(coeff[1])[i],
				ez::value_ptr(coeff[2])[i],
				&roots[count]);
		}
		auto nend = std::remove_if(roots.begin(), roots.begin() + count, [](T val) {
			return val < T(0) || T(1) < val;
		});
		count = nend - roots.begin();

		for (int i = 0; i < count; ++i) {
			*output++ = roots[i];
		}
		return count;
	}

	// Function for finding the cusp on a cubic curve, if it exists.
	template<typename T, glm::length_t N>
	bool findCusp(const glm::vec<N, T>& p0, const glm::vec<N, T>& p1, const glm::vec<N, T>& p2, const glm::vec<N, T>& p3, T& out) {
		using vec_t = typename glm::vec<N, T>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::findCusp requires floating point types!" );
		static_assert(N != 1, "ez::bezier::findCusp does not work for N == 1");
			
		vec_t
			d0 = T(3) * (p1 - p0),
			d1 = T(3) * (p2 - p1),
			d2 = T(3) * (p3 - p2);

		vec_t
			dd0 = T(2) * (d1 - d0),
			dd1 = T(2) * (d2 - d1);

		// This function just doesn't work right now.
		// It needs to calculate the inflection points of the curve, 
		// and if they are close enough to each other, return the average of the two inflections.
		// That will be a cusp.

		return false;
	};

	/*
	template<typename T, glm::length_t N>
	bool findCusp(const glm::vec<N, T>& p0, const glm::vec<N, T>& p1, const glm::vec<N, T>& p2, const glm::vec<N, T>& p3, T& out) {
		using vec_t = typename glm::vec<N, T>;
		static_assert(std::is_floating_point_v<T>, "ez::bezier::findCusp requires floating point types!" );
		static_assert(N != 1, "ez::bezier::findCusp does not work for N == 1");
			
		glm::vec<N, T>
			a = T(3)* (-p0 + T(3)* p1 - T(3)* p2 + p3),
			b = T(6)* (p0 - T(2)* p1 + p2),
			c = T(3)* (p1 - p0);

		std::array<int, 2 * N> dims;
		std::array<T, 2 * N> roots;
		int rcount = 0;
		for (int i = 0; i < N; ++i) {
			int count = ez::poly::solveQuadratic(a[i], b[i], c[i], &roots[rcount]);
			for (int u = 0; u < count; ++u) {
				dims[rcount + u] = i;
			}
			rcount += count;
		}
		if (rcount == 0) {
			return false;
		}

		// Sort the results list, look for N consecutive elements in the list (one from each dim)
		// Simple selection sort, should be very fast for arrays this small.
		for (int i = 0; i < rcount; ++i) {
			int mi = i;
			for (int u = i+1; u < rcount; ++u) {
				if (roots[u] < roots[mi]) {
					mi = u;
				}
			}
			std::swap(roots[i], roots[mi]);
			std::swap(dims[i], dims[mi]);
		}

		static constexpr T eps = ez::epsilon<T>() * T(6);

		// Number of consecutive found
		int count = 1;
		// index of first in consecutives
		int first = 0;
		// Dims passed
		std::array<int, N> passed;
		passed[0] = dims[0];

		for (int i = 1; i < rcount; ++i) {
			// The root is close enough to be valid
			if (std::abs(roots[first] - roots[i]) < eps) {
				// If we have NOT passed this dim already
				if (std::find(passed.begin(), passed.begin() + count, dims[i]) == passed.end()) {
					passed[count] = dims[i];
					++count;

					if (count == N) {
						out = roots[first];
						return true;
					}
				}
			}
			else {
				count = 1;
				first = i;
				passed[0] = dims[i];
			}
		}

		return false;
	};
	*/
};