#pragma once
#include <algorithm>
#include <ez/meta.hpp>
#include <cinttypes>
#include <cassert>
#include <vector>
#include <array>
#include <glm/vec2.hpp>

#include <ez/bezier/intern/BezierInterpolation.hpp>
#include <ez/bezier/intern/BezierLength.hpp>

namespace ez {
	template<typename vec_t>
	struct Control {
		bool corner;
		vec_t position;
	};

	// This class is based on BPath from ez-interpolate
	class ClosedPath {
	public:
		using T = glm::vec2;
		using vec_t = T;
		using real_t = float;

		using size_type = std::size_t;
		using value_type = vec_t;

		using Segment = std::array<vec_t, 4>;
		struct Index {
			size_type index;
			real_t delta;
		};

		using reference = value_type&;
		using pointer = value_type*;
		using const_reference = const value_type&;
		using const_pointer = const value_type*;

		using Container = std::vector<value_type>;

		using iterator = typename Container::iterator;
		using const_iterator = typename Container::const_iterator;
		using reverse_iterator = typename Container::reverse_iterator;
		using const_reverse_iterator = typename Container::const_reverse_iterator;

		~ClosedPath() = default;
		ClosedPath(const ClosedPath&) = default;
		ClosedPath(ClosedPath&&) noexcept = default;
		ClosedPath& operator=(const ClosedPath&) = default;
		ClosedPath& operator=(ClosedPath&&) noexcept = default;

		ClosedPath()
		{}

		ClosedPath(size_type n, const_reference point)
			: points(n, point)
		{}

		// Converts a real value between 0 and 1, into a segment index and an interpolation value for that segment
		Index indexAt(real_t t) const {
			t = std::max(real_t(0), std::max(t, real_t(0.999999)));

			real_t delta, index;
			delta = modf(t * static_cast<real_t>(numSegments()), &index);

			return Index{ static_cast<size_type>(index), delta };
		}

		// Calculate the controls of a cubic bezier curve at the specified index
		Segment segmentAt(size_type i) const {
			assert(i < numSegments());
			Segment seg;

			vec_t p0, p1, p2;

			i = i * 3;

			size_type
				i1 = wrapIndex(i + 1),
				i2 = wrapIndex(i + 2),
				i3 = wrapIndex(i + 3);

			seg[0] = points[i];
			seg[1] = points[i1];
			seg[2] = points[i2];
			seg[3] = points[i3];

			return seg;
		}

		size_type numSegments() const {
			if (points.size() > 2) {
				return points.size() / 3;
			}
			else {
				return 0;
			}
		}

		real_t length() const {
			real_t total = real_t(0);

			size_type count = numSegments();
			for (size_type i = 0; i < count; ++i) {
				Segment seg = segmentAt(i);
				total += ez::bezier::length(seg[0], seg[1], seg[2], seg[3]);
			}

			return total;
		}

		vec_t evalAt(Index index) const {
			Segment seg = segmentAt(index.index);
			return ez::bezier::interpolate(seg[0], seg[1], seg[2], seg[3], index.delta);
		}

		vec_t evalAt(real_t t) const {
			return evalAt(indexAt(t));
		}

		reference operator[](size_type i) {
			assert(i < points.size());
			return points[i];
		}
		const_reference operator[](size_type i) const {
			assert(i < points.size());
			return points[i];
		}
		reference at(size_type i) {
			return points.at(i);
		}
		const_reference at(size_type i) const {
			return points.at(i);
		}

		pointer data() {
			return points.data();
		}
		const_pointer data() const {
			return points.data();
		}

		bool empty() const {
			return points.empty();
		}

		template<typename Iter>
		void assign(Iter first, Iter last) {
			static_assert(ez::is_input_iterator_v<Iter>, "ez::BPath::assign requires an input iterator!");

			points.assign<Iter>(first, last);
		}

		void assign(size_type n, const_reference point) {
			points.assign(n, point);
		}

		void assign(std::initializer_list<value_type> il) {
			points.assign(il);
		}

		void insert(const_iterator it, const_reference point) {
			points.insert(it, point);
		}
		void insert(const_iterator it, size_type n, const_reference point) {
			points.insert(it, n, point);
		}
		template<typename Iter>
		void insert(const_iterator it, Iter first, Iter last) {
			static_assert(ez::is_input_iterator_v<Iter>, "ez::BPath::insert requires an input iterator!");

			points.insert<Iter>(it, first, last);
		}
		void insert(const_iterator it, std::initializer_list<value_type> il) {
			points.insert(it, il);
		}

		void erase(const_iterator it) {
			points.erase(it);
		}
		void erase(const_iterator first, const_iterator last) {
			points.erase(first, last);
		}

		void resize(size_type n) {
			points.resize(n);
		}
		void resize(size_type n, const_reference point) {
			points.resize(n, point);
		}

		size_type size() const {
			return points.size();
		}
		size_type numPoints() const {
			return points.size();
		}
		size_type max_size() const {
			return points.max_size();
		}

		reference front() {
			return points.front();
		}
		const_reference front() const {
			return points.front();
		}

		reference back() {
			return points.back();
		}
		const_reference back() const {
			return points.back();
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

		const_iterator cbegin() const {
			return points.cbegin();
		}
		const_iterator cend() const {
			return points.cend();
		}

		reverse_iterator rbegin() {
			return points.rbegin();
		}
		reverse_iterator rend() {
			return points.rend();
		}

		const_reverse_iterator crbegin() const {
			return points.crbegin();
		}
		const_reverse_iterator crend() const {
			return points.crend();
		}

		void swap(ClosedPath& other) noexcept {
			points.swap(other.points);
		}

		void clear() {
			points.clear();
		}

		void append(const_reference point) {
			points.push_back(point);
		}
		void push_back(const_reference point) {
			points.push_back(point);
		}
		void pop_back() {
			points.pop_back();
		}
	private:
		std::vector<value_type> points;

		// Wrap the index back around to the start of the indices, if it goes past the end
		size_type wrapIndex(size_type i) const noexcept {
			if (i >= numPoints()) {
				i -= numPoints();
			}
			return i;
		}
	};
};