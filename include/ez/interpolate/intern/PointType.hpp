#pragma once
#include <glm/vec2.hpp>

namespace ez {
	namespace intern {
		template<typename T, std::size_t N>
		struct Point {
			using type = typename glm::vec<N, T>;
		};
		template<typename T>
		struct Point<T, 1> {
			using type = T;
		};
	};
}