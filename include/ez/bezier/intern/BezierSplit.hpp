#pragma once
#include <ez/meta.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include "BezierInterpolation.hpp"

namespace ez {
	namespace bezier {
		// Calculate the left side of the split linear curve
		template<typename vec_t, typename U, typename Iter>
		void leftSplit(const vec_t& p0, const vec_t& p1, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::leftSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::leftSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::leftSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::leftSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			*output++ = p0;
			*output++ = interpolate(p0, p1, t);
		}

		// Calculate the right side of the split linear curve
		template<typename vec_t, typename U, typename Iter>
		void rightSplit(const vec_t& p0, const vec_t& p1, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::rightSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::rightSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::rightSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::rightSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;
			
			*output++ = interpolate(p0, p1, t);
			*output++ = p1;
		}

		// Calculate the left side of the split quadratic curve
		template<typename vec_t, typename U, typename Iter>
		void leftSplit(const vec_t& p0, const vec_t& p1, const vec_t& p2, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::leftSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::leftSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::leftSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::leftSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			vec_t lp = interpolate(p0, p1, t);
			vec_t rp = interpolate(p1, p2, t);

			vec_t mp = interpolate(lp, rp, t);

			*output++ = p0;
			*output++ = lp;
			*output++ = mp;
		}

		// Calculate the right side of the split quadratic curve
		template<typename vec_t, typename U, typename Iter>
		void rightSplit(const vec_t& p0, const vec_t& p1, const vec_t& p2, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::rightSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::rightSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::rightSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::rightSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			vec_t lp = interpolate(p0, p1, t);
			vec_t rp = interpolate(p1, p2, t);

			vec_t mp = interpolate(lp, rp, t);

			*output++ = mp;
			*output++ = rp;
			*output++ = p2;
		}

		// Calculate the left side of the split cubic curve
		template<typename vec_t, typename U, typename Iter>
		void leftSplit(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::leftSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::leftSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::leftSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::leftSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			std::array<vec_t, 3> d2{
				interpolate(p0, p1, T(t)),
				interpolate(p1, p2, T(t)),
				interpolate(p2, p3, T(t)),
			};

			std::array<vec_t, 2> d1{
				interpolate(d2[0], d2[1], T(t)),
				interpolate(d2[1], d2[2], T(t)),
			};

			vec_t d0 = interpolate(d1[0], d1[1], T(t));

			*output++ = p0;
			*output++ = d2[0];
			*output++ = d1[0];
			*output++ = d0;
		}

		// Calculate the right side of the split cubic curve
		template<typename vec_t, typename U, typename Iter>
		void rightSplit(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::rightSplit requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::rightSplit requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::rightSplit requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::rightSplit cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			std::array<vec_t, 3> d2{
				interpolate(p0, p1, t),
				interpolate(p1, p2, t),
				interpolate(p2, p3, t),
			};

			std::array<vec_t, 2> d1{
				interpolate(d2[0], d2[1], t),
				interpolate(d2[1], d2[2], t),
			};

			vec_t d0 = interpolate(d1[0], d1[1], t);

			*output++ = d0;
			*output++ = d1[1];
			*output++ = d2[2];
			*output++ = p3;
		}

		// Returns the handles to two cubics created from the input cubic bezier, split at 't'.
		template<typename vec_t, typename U, typename Iter>
		void split(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::split requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::split requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::split requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::split cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;

			std::array<vec_t, 3> d2{
				interpolate(p0, p1, t),
				interpolate(p1, p2, t),
				interpolate(p2, p3, t),
			};

			std::array<vec_t, 2> d1{
				interpolate(d2[0], d2[1], t),
				interpolate(d2[1], d2[2], t),
			};

			glm::vec2 d0 = interpolate(d1[0], d1[1], t);

			*output++ = p0;
			*output++ = d2[0];
			*output++ = d1[0];
			*output++ = d0;
			*output++ = d1[1];
			*output++ = d2[2];
			*output++ = p3;
		}

		// Returns the handles to two Quadratics created from the input quadratic bezier, split at 't'. Creates 5 points, the two segments share a handle.
		template<typename vec_t, typename U, typename Iter>
		void split(const vec_t& p0, const vec_t& p1, const vec_t& p2, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::split requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::split requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::split requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::split cannot write values to output iterator, types are incompatible!");

			using T = vec_value_t<vec_t>;

			vec_t lp = interpolate(p0, p1, t);
			vec_t rp = interpolate(p1, p2, t);

			vec_t mp = interpolate(lp, rp, t);

			*output++ = p0;
			*output++ = lp;
			*output++ = mp;
			*output++ = rp;
			*output++ = p2;
		}

		// Returns the handles to two linears created from the input linear bezier, split at 't'. Creates 3 points, the two segments share a handle.
		template<typename vec_t, typename U, typename Iter>
		void split(const vec_t& p0, const vec_t& p1, U t, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::split requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::split requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::split requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::split cannot write values to output iterator, types are incompatible!");

			using T = vec_value_t<vec_t>;

			*output++ = p0;
			*output++ = interpolate(p0, p1, t);
			*output++ = p1;
		}
	
		// Returns the linear segment between 't0' and 't1'
		// 't1' must be greater than or equal to 't0'
		template<typename vec_t, typename U, typename  Iter>
		void segment(const vec_t& p0, const vec_t& p1, U t0, U t1, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::segment requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::segment requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::segment requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::segment cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;
			constexpr std::size_t N = 2;

			// Precision drops when the epsilon is too low and t1 is close to zero
			constexpr T eps = ez::epsilon<T>() * T(10);

			// Avoid divide by zero
			if (std::abs(t1) > eps) {
				t0 = t0 / t1;

				std::array<vec_t, N> tmp;
				leftSplit(p0, p1, tmp.begin());
				rightSplit(tmp[0], tmp[1], t0, output);
			}
			else {
				leftSplit(p0, p1, t0, output);
			}
		};

		// Returns the quadratic segment between 't0' and 't1'
		// 't1' must be greater than or equal to 't0'
		template<typename vec_t, typename U, typename  Iter>
		void segment(const vec_t& p0, const vec_t& p1, const vec_t& p2, U t0, U t1, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::segment requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::segment requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::segment requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::segment cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;
			constexpr std::size_t N = 3;

			// Precision drops when the epsilon is too low and t1 is close to zero
			constexpr T eps = ez::epsilon<T>() * T(10);

			// Avoid divide by zero
			if (std::abs(t1) > eps) {
				t0 = t0 / t1;

				std::array<vec_t, N> tmp;
				leftSplit(p0, p1, p2, t1, tmp.begin());
				rightSplit(tmp[0], tmp[1], tmp[2], t0, output);
			}
			else {
				leftSplit(p0, p1, p2, t0, output);
			}
		};

		// Returns the cubic segment between 't0' and 't1'
		// 't1' must be greater than or equal to 't0'
		template<typename vec_t, typename U, typename  Iter>
		void segment(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t p3, U t0, U t1, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::segment requires vector types!");
			static_assert(std::is_floating_point_v<U>, "ez::bezier::segment requires floating point types!");
			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::segment requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::segment cannot write values to output iterator, types are incompatible!");
			using T = vec_value_t<vec_t>;
			constexpr std::size_t N = 4;

			// Precision drops when the epsilon is too low and t1 is close to zero
			constexpr T eps = ez::epsilon<T>() * T(10);

			// Avoid divide by zero
			if(std::abs(t1) > eps) {
				t0 = t0 / t1;

				std::array<vec_t, N> tmp;
				leftSplit(p0, p1, p2, p3, t1, tmp.begin());
				rightSplit(tmp[0], tmp[1], tmp[2], tmp[3], t0, output);
			}
			else {
				leftSplit(p0, p1, p2, p3, t0, output);
			}
		};

		// The expanders take iterators and expand them into arguments for the above functions
		// This makes it easier to do certain kinds of meta-programming.
		namespace intern {
			template<typename input_iter, typename T, typename output_iter>
			struct LeftSplitExpander {
				input_iter input;
				output_iter output;
				T t;

				template<std::size_t N, typename ...Ts>
				void call(Ts&&... args) {
					if constexpr (N == 0) {
						return bezier::leftSplit(std::forward<Ts>(args)..., t, output);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T, typename output_iter>
			struct RightSplitExpander {
				input_iter input;
				output_iter output;
				T t;

				template<std::size_t N, typename ...Ts>
				void call(Ts&&... args) {
					if constexpr (N == 0) {
						return bezier::rightSplit(std::forward<Ts>(args)..., t, output);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T, typename output_iter>
			struct SplitExpander {
				input_iter input;
				T t;
				output_iter output;

				template<std::size_t N, typename ...Ts>
				void call(Ts&&... args) {
					if constexpr (N == 0) {
						return bezier::split(std::forward<Ts>(args)..., t, output);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T, typename output_iter>
			struct SegmentExpander {
				input_iter input;
				T t0, t1;
				output_iter output;

				template<std::size_t N, typename ...Ts>
				void call(Ts&&... args) {
					if constexpr (N == 0) {
						return bezier::segment(std::forward<Ts>(args)..., t0, t1, output);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};
		}

		template<std::size_t N, typename input_iter, typename T, typename output_iter>
		void leftSplitStatic(input_iter input, T t, output_iter output) {
			static_assert(N >= 2 && N <= 4, "ez::bezier::leftSplitStatic currently only allows for N in range [2, 4]!");
			intern::LeftSplitExpander<input_iter, T, output_iter>{input, output, t}.call<N>();
		};
		template<std::size_t N, typename input_iter, typename T, typename output_iter>
		void rightSplitStatic(input_iter input, T t, output_iter output) {
			static_assert(N >= 2 && N <= 4, "ez::bezier::rightSplitStatic currently only allows for N in range [2, 4]!");
			intern::RightSplitExpander<input_iter, T, output_iter>{input, output, t}.call<N>();
		};
		template<std::size_t N, typename input_iter, typename T, typename output_iter>
		void splitStatic(input_iter input, T t, output_iter output) {
			static_assert(N >= 2 && N <= 4, "ez::bezier::splitStatic currently only allows for N in range [2, 4]!");
			intern::SplitExpander<input_iter, T, output_iter>{input, output}.call<N>();
		};
		template<std::size_t N, typename input_iter, typename T, typename output_iter>
		void segmentStatic(input_iter input, T t0, T t1, output_iter output) {
			static_assert(N >= 2 && N <= 4, "ez::bezier::segmentStatic currently only allows for N in range [2, 4]!");
			intern::SegmentExpander<input_iter, T, output_iter>{input, t0, t1, output}.call<N>();
		};
	};
}