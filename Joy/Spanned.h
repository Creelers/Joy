#pragma once

#include "Common.h"

namespace syntax {
	struct Position {
		u32 line;
		u32 column;
	};

	struct Span {
		Position start;
		Position end;
	};

	inline Span span_to(Position start, u32 line, u32 column) {
		return Span{ start, {line, column} };
	}

	template<typename T> struct Spanned {
		Span span;
		T value;
	};
}