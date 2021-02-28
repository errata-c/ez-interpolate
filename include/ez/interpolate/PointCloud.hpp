#pragma once
#include <ez/meta.hpp>
#include <ez/math/constants.hpp>

#include <vector>
#include <cinttypes>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <array>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ez {

	/*
	Interpolation using a cloud of points.
	There can be as many input dimensions as needed.
	Manipulating point clouds locally is easier than manipulating bezier curves, its just the transistions to new region on the cloud that is more difficult
	Derivatives can be calculated easily using automatic differentiation.
	*/
	template<typename T, int InDim, int OutDim>
	class PointCloud {
	public:
		static_assert(InDim > 0 && InDim <= 4, "Input Dimension is out of range!");
		static_assert(OutDim > 0 && OutDim <= 4, "Output Dimension is out of range!");

		using real_t = T;
		using ovec = std::conditional_t<OutDim == 1, T, glm::vec<OutDim, T>>;
		using ivec = std::conditional_t<InDim == 1, T, glm::vec<InDim, T>>;

		struct Dimension;
		using index_t = std::intptr_t;
		using size_t = std::size_t;

		struct Point {
			Point()
				: position{ T(0) }
				, domain{ T(1) }
				, tangent{ ovec{ T(0) } }
			{}

			Point(ivec p, ovec v)
				: position{ p }
				, domain{ T(1) }
				, tangent{ ovec{ T(0) } }
				, output(v)
			{}

			// The interpolation value this knot is resting at.
			ivec position;

			// The scale factor for this knots input value.
			ivec domain;
			
			// Tangents along each input dimension.
			std::array<ovec, InDim> tangent;

			// The output point at this input position.
			ovec output;
		private:
			mutable real_t cache;
			friend class PointCloud;
		};

		using container_t = std::vector<Point>;
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;
		
		PointCloud(const PointCloud&) = default;
		PointCloud(PointCloud&&) noexcept = default;
		PointCloud& operator=(const PointCloud&) = default;
		PointCloud& operator=(PointCloud&&) noexcept = default;
		
		PointCloud()
		{}

		PointCloud(std::size_t cap)
			: points{cap}
		{}

		static constexpr int inputDimensions() {
			return InDim;
		}
		static constexpr int outputDimensions() {
			return OutDim;
		}

		void push_back(ivec input, ovec output) {
			points.push_back(Point(input, output));
		}
		void append(ivec input, ovec output) {
			points.push_back(Point(input, output));
		}
		void pop_back() {
			points.pop_back();
		}
		void erase(const_iterator iter) {
			points.erase(iter);
		}
		void insert(iterator iter, const Point & value) {
			points.insert(iter, value);
		}
		void resize(std::size_t count) {
			points.resize(count);
		}

		Point& operator[](std::size_t index) {
			assert(index < size());
			return points[index];
		}
		const Point& operator[](std::size_t index) const {
			assert(index < size());
			return points[index];
		}

		std::size_t size() const {
			return points.size();
		}
		std::size_t numPoints() const {
			return points.size();
		}

		void clear() {
			points.clear();
		}

		iterator begin() {
			return points.begin();
		}
		iterator end() {
			return points.end();
		}

		const_iterator begin() const {
			return points.begin();
		}
		const_iterator end() const {
			return points.end();
		}

		ovec eval(ivec input) const {
			ovec result(0);
			real_t sum = real_t(0);

			// We calculate the coefficient, add it to the sum, then cache the coefficient for the second pass
			for (const Point& point : points) {
				ivec delta = input - point.position;

				delta = point.domain * delta * delta;

				real_t coeff = 0.f;
				if constexpr (InDim == 1) {
					coeff = delta;
				}
				else {
					for (int i = 0; i < InDim; ++i) {
						coeff += delta[i];
					}
				}

				// We add an epsilion value to prevent division by zero
				coeff = real_t(1) / (ez::epsilon<real_t>() + coeff);

				sum += coeff;
				point.cache = coeff;
			}

			// normalize the influence factors, sum result vector
			for (const Point& point : points) {
				ivec delta = input - point.position;

				// normalization, note that if sum == inf, then most likely at least one point.cache == inf.
				// So the factor will == 1 for at least one point.
				real_t factor = point.cache / sum;

				ovec offset{0.f};
				if constexpr (InDim == 1) {
					offset = point.tangent[0] * delta;
				}
				else {
					for (int i = 0; i < InDim; ++i) {
						offset += point.tangent[i] * delta[i];
					}
				}

				result += (point.output + offset) * factor;
			}
			return result;
		}

	private:
		std::vector<Point> points;
	};
};