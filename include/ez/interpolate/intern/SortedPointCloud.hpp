#pragma once
#include <vector>
#include <cinttypes>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include "intern/PointType.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ez {
	namespace intern {
		template<typename real_t, typename vec_t>
		struct Dist {
			real_t value;
			vec_t position;
		};
		template<typename real_t, typename vec_t>
		static bool operator<(const Dist<real_t, vec_t>& left, const Dist<real_t, vec_t>& right) {
			return left.value < right.value;
		};
		template<typename real_t, typename vec_t>
		static bool operator>(const Dist<real_t, vec_t>& left, const Dist<real_t, vec_t>& right) {
			return left.value > right.value;
		};
	};

	/*
	Interpolation using a cloud of points.
	There can be as many input dimensions as needed.
	Manipulating point clouds locally is easier than manipulating bezier curves, its just the transistions to new region on the cloud that is more difficult
	Derivatives can be calculated easily using automatic differentiation.
	*/
	template<typename T, std::size_t N>
	class SortedPointCloud {
	public:
		struct Dimension;
		using index_t = typename std::intptr_t;
		using size_type = index_t;
		using real_t = T;
		using vec_t = typename intern::Point<T, N>::type;

		struct Knot {
			Knot()
				: position(0)
				, domain(1)
				, tangent(0)
			{}

			// The interpolation value this knot is resting at. (input offset essentially)
			real_t position;

			// The scale factor for this knots input value.
			real_t domain;

			// The tangent for this knot, defines the direction the evaluated position is offset when approaching this knot.
			vec_t tangent;
		};
		class Point {
		public:
			using iterator = typename std::vector<Knot>::iterator;
			using const_iterator = typename std::vector<Knot>::const_iterator;

			Point()
				: value(0)
			{}
			Point(vec_t val, index_t count)
				: value(val)
				, knots(count)
			{}

			vec_t value;

			Knot& operator[](std::intptr_t index) {
				assert(index >= 0 && index < size());
				return knots[index];
			}
			const Knot& operator[](std::intptr_t index) const {
				assert(index >= 0 && index < size());
				return knots[index];
			}
			index_t size() const {
				return static_cast<std::intptr_t>(knots.size());
			}

			iterator begin() {
				return knots.begin();
			}
			iterator end() {
				return knots.end();
			}
			
			const_iterator begin() const {
				return knots.begin();
			}
			const_iterator end() const {
				return knots.end();
			}
		private:
			friend class SortedPointCloud;
			std::vector<Knot> knots;
		};

		using container_t = std::vector<Point>;
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;
		
		SortedPointCloud(const SortedPointCloud&) = default;
		SortedPointCloud(SortedPointCloud&&) noexcept = default;
		SortedPointCloud& operator=(const SortedPointCloud&) = default;
		SortedPointCloud& operator=(SortedPointCloud&&) noexcept = default;
		
		SortedPointCloud()
			: inputs(0)
		{}

		SortedPointCloud(index_t pointCount, index_t inputCount)
			: inputs(inputCount)
			, points(pointCount, Point(vec_t{0}, inputCount))
		{}

		void setKnot(index_t pointIndex, index_t inputIndex, Knot value) {
			getKnot(pointIndex, inputIndex) = value;
		}

		Knot& getKnot(index_t pointIndex, index_t inputIndex) {
			assert(pointIndex >= 0 && pointIndex < numPoints());
			assert(inputIndex >= 0 && inputIndex < numInputs());

			return points[pointIndex].knots[inputIndex];
		}
		const Knot& getKnot(index_t pointIndex, index_t inputIndex) const {
			assert(pointIndex >= 0 && pointIndex < numPoints());
			assert(inputIndex >= 0 && inputIndex < numInputs());

			return points[pointIndex].knots[inputIndex];
		}

		void push_back(vec_t value) {
			points.push_back(Point(value, inputs));
		}
		void append(vec_t value) {
			points.push_back(Point(value, inputs));
		}
		void pop_back() {
			points.pop_back();
		}
		void erase(const_iterator iter) {
			points.erase(iter);
		}
		void insert(iterator iter, vec_t value) {
			points.insert(iter, Point(value, inputs));
		}
		void resizePoints(index_t count) {
			assert(count >= 0);

			index_t excess = count - points.size();
			if (excess < 0) { // There are now less points.
				points.resize(count);
			}
			else { // There are now more points
				for (index_t i = 0; i < excess; ++i) {
					points.push_back(Point{ vec_t{0}, inputs });
				}
			}
		}

		void resizeInputs(index_t count) {
			assert(count >= 0);
			
			for (Point& point : points) {
				point.knots.resize(count);
			}
			inputs = count;
		}

		Point& operator[](std::intptr_t index) {
			assert(index >= 0 && index < numPoints());
			return points[index];
		}
		const Point& operator[](std::intptr_t index) const {
			assert(index >= 0 && index < numPoints());
			return points[index];
		}

		vec_t& getPoint(index_t index) {
			assert(index >= 0 && index < numPoints());
			return points[index].value;
		}
		const vec_t& getPoint(index_t index) const {
			assert(index >= 0 && index < numPoints());
			return points[index].value;
		}
		void setPoint(index_t index, vec_t point) {
			assert(index >= 0 && index < numPoints());
			points[index].value = point;
		}

		index_t size() const {
			return static_cast<index_t>(points.size());
		}
		index_t numPoints() const {
			return static_cast<index_t>(points.size());
		}
		index_t numInputs() const {
			return inputs;
		}

		void clear() {
			points.clear();
			inputs = 0;
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
	private:
		index_t inputs;
		// The interpolation points.
		std::vector<Point> points;

		template<typename Iter>
		vec_t evalImpl(Iter inFirst, Iter inLast) const {
			using namespace ez::intern;

			vec_t result(0);
			real_t sum = real_t(0);

			// Calculate all the distances
			for (const_iterator pointIter = begin(), last = end(); pointIter != last; ++pointIter) {
				real_t value = real_t(0);
				vec_t position = pointIter->value;

				Iter it = inFirst;
				for (const Knot& knot : pointIter->knots) {
					assert(it != inLast);
					
					// The domain scaling occurs only along this particular knots dimension.
					real_t tmp = (knot.position - *it);
					tmp = tmp * tmp;
					value += knot.domain * tmp;
					
					tmp =  real_t(1) - real_t(1) / (tmp * tmp + real_t(1));

					position += knot.tangent * tmp;

					++it;
				}

				value = real_t(1) / (real_t(0.00001) + value);

				result += position * value;
				sum += value;
			}

			if (sum >= real_t(1E-5)) {
				result /= sum;
			}
			
			return result;
		}
	public:
		vec_t eval(real_t * begin, real_t * end) const {
			return evalImpl(begin, end);
		}

		template<typename Iter>
		vec_t eval(Iter begin, Iter end) const {
			static_assert(std::is_convertible<typename Iter::value_type, real_t>::value, "The iterator passed into ez::PointCloud does not have convertible value type.");
			return evalImpl(begin, end);
		}
	};
};