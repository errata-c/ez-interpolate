#pragma once
#include <ez/meta.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

namespace ez {
	namespace bezier {
		// Line interpolation
		template<typename vec_t>
		vec_t interpolate(const vec_t& p0, const vec_t& p1, vec_value_t<vec_t> t) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::interpolate requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::interpolate requires floating point types!");

			return p0 * (T(1) - t) + p1 * t;
		};

		// Quadratic interpolation
		template<typename vec_t>
		vec_t interpolate(const vec_t& p0, const vec_t& p1, const vec_t& p2, vec_value_t<vec_t> t) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::interpolate requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::interpolate requires floating point types!");

			T t1 = T(1) - t;

			return (p0 * t1 * t1) + (p1 * T(2) * t1 * t) + (p2 * t * t);
		};

		// Cubic interpolation
		template<typename vec_t>
		vec_t interpolate(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, vec_value_t<vec_t> t) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::interpolate requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::interpolate requires floating point types!");

			T t1 = T(1) - t;

			T tt = t * t;
			T t1t1 = t1 * t1;

			return p0 * t1t1 * t1 + p1 * T(3)* t1t1* t + p2 * T(3)* tt* t1 + p3 * tt * t;
		};

		
		template<typename T, typename Iter>
		ez::iterator_value_t<Iter> interpolateRange(Iter begin, Iter end, T t) {
			static_assert(ez::is_random_iterator_v<Iter>, "ez::bezier::interpolateRange requires an random access iterator!");
			using vec_t = ez::iterator_value_t<Iter>;
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::interpolateRange requires iterator to vector type!");

			intptr_t diff = end - begin;
			switch (diff) {
			case 0:
				return vec_t{0};
			case 1:
				return *begin;
			case 2: // linear
				return bezier::interpolate(*(begin), *(begin + 1), t);
			case 3: // quadratic
				return bezier::interpolate(*(begin), *(begin + 1), *(begin + 2), t);
			case 4: // cubic
				return bezier::interpolate(*(begin), *(begin + 1), *(begin + 2), *(begin + 3), t);
			default:
				// Dynamic:
				// Recursively evaluate the bezier from the input points.
				return bezier::interpolate(bezier::interpolate(begin, end - 1, t), bezier::interpolate(begin + 1, end, t), t);
			}
		};

		namespace intern {
			// Expands an input iterator into a parameter pack, to call one of the above interpolate functions
			// The only way to do this easily is to capture the input iterator and the t value.
			template<typename T, typename Iter>
			struct InterpExpander {
				using vec_t = ez::iterator_value_t<Iter>;

				Iter iter;
				T t;

				template<std::size_t N, typename ...Ts>
				vec_t call(Ts&&... args) {
					if constexpr (N == 0) {
						return bezier::interpolate(std::forward<Ts>(args)..., t);
					}
					else {
						return call<N-1>(std::forward<Ts>(args)..., *iter++);
					}
				}
			};
		}
		template<std::size_t N, typename T, typename Iter>
		ez::iterator_value_t<Iter> interpolateStatic(Iter begin, T t) {
			static_assert(N <= 4, "interpolateStatic does not support values of N greater than 4");
			return intern::InterpExpander<T, Iter>{begin, t}.call<N>();
		};

		/*
		template<typename T, typename Iter, std::enable_if_t<is_input_iterator<Iter>::value && std::is_floating_point<T>::value, int> = 0>
		typename Iter::value_type surfaceInterpolate(Iter row0, Iter row1, T u, T v) {
			using vec_t = Iter::value_type;

			T v1 = T(1) - v;
			T u1 = T(1) - u;

			T uc[2];
			T vc[2];

			uc[0] = u1;
			uc[1] = u;

			vc[0] = v1;
			vc[1] = v;

			vec_t arr[2][2];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;

			vec_t ret{ T(0) };

			for (int y = 0; y < 2; ++y) {
				for (int x = 0; x < 2; ++x) {
					ret += arr[x][y] * uc[x] * vc[y];
				}
			}

			return ret;
		};

		template<typename T, typename Iter, std::enable_if_t<is_input_iterator<Iter>::value && std::is_floating_point<T>::value, int> = 0>
		typename Iter::value_type surfaceInterpolate(Iter row0, Iter row1, Iter row2, T u, T v) {
			using vec_t = typename Iter::value_type;

			T v1 = T(1) - v;
			T u1 = T(1) - u;

			T uc[3];
			T vc[3];

			uc[0] = u1 * u1;
			uc[1] = u * u1;
			uc[2] = u * u;

			vc[0] = v1 * v1;
			vc[1] = v * v1;
			vc[2] = v * v;

			vec_t arr[3][3];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;

			vec_t ret{T(0)};

			for (int y = 0; y < 3; ++y) {
				for (int x = 0; x < 3; ++x) {
					ret += arr[x][y] * uc[x] * vc[y];
				}
			}

			return ret;
		};

		template<typename T, typename Iter, std::enable_if_t<is_input_iterator<Iter>::value && std::is_floating_point<T>::value, int> = 0>
		typename Iter::value_type surfaceInterpolate(Iter row0, Iter row1, Iter row2, Iter row3, T u, T v) {
			using vec_t = typename Iter::value_type;

			T v1 = T(1) - v;
			T u1 = T(1) - u;

			T uc[4];
			T vc[4];

			uc[0] = u1 * u1 * u1;
			uc[1] = u * u1 * u1 * T(3);
			uc[2] = u * u * u1 * T(3);
			uc[3] = u * u * u;

			vc[0] = v1 * v1 * v1;
			vc[1] = v * v1 * v1 * T(3);
			vc[2] = v * v * v1 * T(3);
			vc[3] = v * v * v;

			vec_t arr[4][4];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;
			++row0;
			arr[0][3] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;
			++row1;
			arr[1][3] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;
			++row2;
			arr[2][3] = *row2;

			arr[3][0] = *row3;
			++row3;
			arr[3][1] = *row3;
			++row3;
			arr[3][2] = *row3;
			++row3;
			arr[3][3] = *row3;

			vec_t ret{ T(0) };

			for (int y = 0; y < 4; ++y) {
				for (int x = 0; x < 4; ++x) {
					ret += arr[x][y] * uc[x] * vc[y];
				}
			}

			return ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveU(InIter row0, InIter row1, T u, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");
			
			T u1 = T(1) - u;

			T uc[2];

			uc[0] = u1;
			uc[1] = u;
			
			vec_t arr[2][2];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;

			vec_t ret{ T(0) };
			for (int x = 0; x < 2; ++x) {
				ret += arr[x][0] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 2; ++x) {
				ret += arr[x][0] * uc[x];
			}
			(*output) = ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveV(InIter row0, InIter row1, T v, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");

			T v1 = T(1) - v;

			T vc[2];

			vc[0] = v1;
			vc[1] = v;

			vec_t arr[2][2];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;

			vec_t ret{ T(0) };
			for (int y = 0; y < 2; ++y) {
				ret += arr[0][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 2; ++y) {
				ret += arr[0][y] * vc[y];
			}
			(*output) = ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveU(InIter row0, InIter row1, InIter row2, T u, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");

			T u1 = T(1) - u;

			T uc[3];

			uc[0] = u1 * u1;
			uc[1] = u * u1;
			uc[2] = u * u;

			vec_t arr[3][3];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;

			vec_t ret{ T(0) };
			for (int x = 0; x < 3; ++x) {
				ret += arr[x][0] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 3; ++x) {
				ret += arr[x][1] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 3; ++x) {
				ret += arr[x][2] * uc[x];
			}
			(*output) = ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveV(InIter row0, InIter row1, InIter row2, T v, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");

			T v1 = T(1) - v;

			T vc[3];

			vc[0] = v1 * v1;
			vc[1] = v * v1;
			vc[2] = v * v;

			vec_t arr[3][3];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;

			vec_t ret{ T(0) };
			for (int y = 0; y < 3; ++y) {
				ret += arr[0][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 3; ++y) {
				ret += arr[1][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 3; ++y) {
				ret += arr[2][y] * vc[y];
			}
			(*output) = ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveU(InIter row0, InIter row1, InIter row2, InIter row3, T u, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");

			T u1 = T(1) - u;

			T uc[4];

			uc[0] = u1 * u1 * u1;
			uc[1] = u * u1 * u1 * T(3);
			uc[2] = u * u * u1 * T(3);
			uc[3] = u * u * u;

			vec_t arr[4][4];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;
			++row0;
			arr[0][3] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;
			++row1;
			arr[1][3] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;
			++row2;
			arr[2][3] = *row2;

			arr[3][0] = *row3;
			++row3;
			arr[3][1] = *row3;
			++row3;
			arr[3][2] = *row3;
			++row3;
			arr[3][3] = *row3;

			vec_t ret{ T(0) };
			for (int x = 0; x < 4; ++x) {
				ret += arr[x][0] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 4; ++x) {
				ret += arr[x][1] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 4; ++x) {
				ret += arr[x][2] * uc[x];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int x = 0; x < 4; ++x) {
				ret += arr[x][3] * uc[x];
			}
			(*output) = ret;
		};

		// Find a subcurve along the surface.
		template<typename T, typename InIter, typename OutIter>
		void surfaceSubcurveV(InIter row0, InIter row1, InIter row2, InIter row3, T v, OutIter output) {
			static_assert(is_random_iterator<InIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<InIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<T>::value, "The interpolation value passed in must be a floating point value!");

			T v1 = T(1) - v;

			T vc[4];

			vc[0] = v1 * v1 * v1;
			vc[1] = v * v1 * v1 * T(3);
			vc[2] = v * v * v1 * T(3);
			vc[3] = v * v * v;

			vec_t arr[4][4];

			arr[0][0] = *row0;
			++row0;
			arr[0][1] = *row0;
			++row0;
			arr[0][2] = *row0;
			++row0;
			arr[0][3] = *row0;

			arr[1][0] = *row1;
			++row1;
			arr[1][1] = *row1;
			++row1;
			arr[1][2] = *row1;
			++row1;
			arr[1][3] = *row1;

			arr[2][0] = *row2;
			++row2;
			arr[2][1] = *row2;
			++row2;
			arr[2][2] = *row2;
			++row2;
			arr[2][3] = *row2;

			arr[3][0] = *row3;
			++row3;
			arr[3][1] = *row3;
			++row3;
			arr[3][2] = *row3;
			++row3;
			arr[3][3] = *row3;

			vec_t ret{ T(0) };
			for (int y = 0; y < 4; ++y) {
				ret += arr[0][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 4; ++y) {
				ret += arr[1][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 4; ++y) {
				ret += arr[2][y] * vc[y];
			}
			(*output) = ret;
			++output;

			ret = vec_t{ T(0) };
			for (int y = 0; y < 4; ++y) {
				ret += arr[3][y] * vc[y];
			}
			(*output) = ret;
		};
		*/
	};
};