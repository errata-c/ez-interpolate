#pragma once
#include <ez/meta.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

namespace ez {
	namespace bezier {
		// Linear bezier derivative.
		template<typename vec_t>
		vec_t derivativeAt(const vec_t& p0, const vec_t& p1, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::derivativeAt requires a vector type as input!");
			static_assert(std::is_floating_point_v<vec_value_t<vec_t>>, "ez::bezier::derivativeAt requires floating point types!");

			return p1 - p0;
		};
		// Quadratic bezier derivative.
		template<typename vec_t>
		vec_t derivativeAt(const vec_t& p0, const vec_t& p1, const vec_t& p2, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::derivativeAt requires a vector type as input!");
			
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::derivativeAt requires floating point types!");

			return interpolate(T(2) * (p1 - p0), T(2) * (p2 - p1), t);
		};
		// Cubic bezier derivative.
		template<typename vec_t>
		vec_t derivativeAt(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::derivativeAt requires a vector type as input!");

			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::derivativeAt requires floating point types!");

			return interpolate(T(3) * (p1 - p0), T(3) * (p2 - p1), T(3) * (p3 - p2), t);
		};

		template<typename vec_t>
		vec_t tangentAt(const vec_t& p0, const vec_t& p1, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::tangent requires a vector type as input!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::tangent requires floating point types!");

			return glm::normalize(p1 - p0);
		};
		template<typename T>
		glm::tvec2<T> normalAt(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, T t) {
			static_assert(std::is_floating_point_v<T>, "ez::bezier::normal requires floating point types!");

			glm::tvec2<T> tmp = tangentAt(p0, p1, t);
			return glm::tvec2<T>{ -tmp.y, tmp.x };
		};

		template<typename vec_t>
		vec_t tangentAt(const vec_t& p0, const vec_t& p1, const vec_t& p2, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::tangent requires a vector type as input!");

			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::tangent requires floating point types!");

			return glm::normalize(derivativeAt(p0, p1, p2, t));
		};

		template<typename T>
		glm::tvec2<T> normalAt(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T t) {
			static_assert(std::is_floating_point_v<T>, "ez::bezier::normal requires floating point types!");

			glm::tvec2<T> tmp = tangentAt(p0, p1, p2, t);
			return glm::tvec2<T>{ -tmp.y, tmp.x };
		};

		template<typename vec_t>
		vec_t tangentAt(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, vec_value_t<vec_t> t) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::tangent requires a vector type as input!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::tangent requires floating point types!");

			return glm::normalize(derivativeAt(p0, p1, p2, p3, t));
		};
		template<typename T>
		glm::tvec2<T> normalAt(const glm::tvec2<T>& p0, const glm::tvec2<T>& p1, const glm::tvec2<T>& p2, const glm::tvec2<T>& p3, T t) {
			static_assert(std::is_floating_point_v<T>, "ez::bezier::normal requires floating point types!");

			glm::tvec2<T> tmp = tangentAt(p0, p1, p2, p3, t);
			return glm::tvec2<T>{ -tmp.y, tmp.x };
		};



		template<typename vec_t, typename Iter>
		void derivative(const vec_t& p0, const vec_t& p1, Iter output) {
			static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			*output++ = p1 - p0;
		};
		template<typename vec_t, typename Iter>
		void derivative(const vec_t& p0, const vec_t& p1, const vec_t& p2, Iter output) {
			static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			using T = vec_value_t <vec_t>;

			*output++ = T(2) * (p1 - p0);
			*output++ = T(2) * (p2 - p1);
		};
		template<typename vec_t, typename Iter>
		void derivative(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3, Iter output) {
			static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			using T = vec_value_t<vec_t>;

			*output++ = T(3) * (p1 - p0);
			*output++ = T(3) * (p2 - p1);
			*output++ = T(3) * (p3 - p2);
		};

		namespace intern {
			template<typename input_iter, typename output_iter>
			struct DerivativeExpander {
				input_iter input;
				output_iter output;

				template<std::size_t N, typename ... Ts>
				void call(Ts&&... args) {
					if constexpr (N == 0) {
						derivative(std::forward<Ts>(args)..., output);
					}
					else {
						call<N-1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T>
			struct DerivativeAtExpander {
				using vec_t = ez::iterator_value_t<input_iter>;

				input_iter input;
				T t;

				template<std::size_t N, typename ... Ts>
				vec_t call(Ts&&... args) {
					if constexpr (N == 0) {
						return derivativeAt(std::forward<Ts>(args)..., t);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T>
			struct TangentAtExpander {
				using vec_t = ez::iterator_value_t<input_iter>;

				input_iter input;
				T t;

				template<std::size_t N, typename ... Ts>
				vec_t call(Ts&&... args) {
					if constexpr (N == 0) {
						return tangentAt(std::forward<Ts>(args)..., t);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};

			template<typename input_iter, typename T>
			struct NormalAtExpander {
				using vec_t = ez::iterator_value_t<input_iter>;

				input_iter input;
				T t;

				template<std::size_t N, typename ... Ts>
				vec_t call(Ts&&... args) {
					if constexpr (N == 0) {
						return normalAt(std::forward<Ts>(args)..., t);
					}
					else {
						return call<N - 1>(std::forward<Ts>(args)..., *input++);
					}
				}
			};
		}

		template<std::size_t N, typename input_iter, typename T, typename output_iter>
		void derivativeStatic(input_iter input, output_iter output) {
			intern::DerivativeExpander<input_iter, output_iter>{input, output}.call<N>();
		}
		template<std::size_t N, typename input_iter, typename T>
		ez::iterator_value_t<input_iter> derivativeAtStatic(input_iter input, T t) {
			return intern::DerivativeAtExpander<input_iter, T>{input, t}.call<N>();
		}
		template<std::size_t N, typename input_iter, typename T>
		ez::iterator_value_t<input_iter> tangentAtStatic(input_iter input, T t) {
			return intern::TangentAtExpander<input_iter, T>{input, t}.call<N>();
		}
		template<std::size_t N, typename input_iter, typename T>
		ez::iterator_value_t<input_iter> normalAtStatic(input_iter input, T t) {
			return intern::NormalAtExpander<input_iter, T>{input, t}.call<N>();
		}

		/*
		template<typename T, typename Iter>
		typename Iter::value_type tangentRange(Iter begin, Iter end, T t) {
			static_assert(is_random_iterator_v<Iter>, "ez::bezier::tangentRange requires a random access iterator!");
			static_assert(is_iterator_readable_v<Iter>, "ez::bezier::tangentRange must have iterator with vector value_type!");
			
			using vec_t = iterator_value_t<Iter>;

			std::intptr_t diff = end - begin;
			switch (diff) {
			case 0:
			case 1:
				return vec_t{};
			case 2: // linear
				return glm::normalize(*(begin + 1) - *(begin));
			case 3: // quadratic
				return glm::normalize(bezier::interpolate(*(begin + 1), *(begin + 2), t) - bezier::interpolate(*(begin), *(begin + 1), t));
			case 4: // cubic
				return glm::normalize(bezier::interpolate(*(begin + 1), *(begin + 2), *(begin + 3), t) - bezier::interpolate(*(begin), *(begin + 1), *(begin + 2), t));
			default:
				// Dynamic:
				return glm::normalize(bezier::interpolate(begin + 1, end, t) - bezier::interpolate(begin, end - 1, t));
			}
		};
		template<typename T, typename Iter>
		typename Iter::value_type normalRange(Iter begin, Iter end, T t) {
			static_assert(is_random_iterator_v<Iter>, "ez::bezier::tangentRange requires a random access iterator!");
			
			using vec_t = iterator_value_t<Iter>;
			static_assert(is_vec_v<vec_t>, "ez::bezier::tangentRange must have iterator with vector value_type!");
	
			static_assert(is_vec<vec_t>::length == 2, "ez::bezier::normal function requires a two dimensional vector.");

			std::intptr_t diff = end - begin;
			vec_t tmp{ 1, 0 };
			switch (diff) {
			case 0:
				assert(false); // Zero length iterator range passed in.
				break;
			case 1:
				assert(false); // Impossible to find a normal when given just one point.
				break;
			case 2: // linear
				tmp = glm::normalize(*(begin + 1) - *(begin));
				break;
			case 3: // quadratic
				tmp = glm::normalize(bezier::interpolate(*(begin + 1), *(begin + 2), t) - bezier::interpolate(*(begin), *(begin + 1), t));
				break;
			case 4: // cubic
				tmp = glm::normalize(bezier::interpolate(*(begin + 1), *(begin + 2), *(begin + 3), t) - bezier::interpolate(*(begin), *(begin + 1), *(begin + 2), t));
				break;
			default:
				// Dynamic:
				tmp = glm::normalize(bezier::interpolate(begin + 1, end, t) - bezier::interpolate(begin, end - 1, t));
				break;
			}

			tmp = vec_t{ -tmp.y, tmp.x };
			return tmp;
		};

		
		*/
	};
};
