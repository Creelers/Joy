#pragma once

#include "Common.h"
#include "Typed.h"

namespace front {
	class TypeChecker {
	public:
		std::unique_ptr<typed::Program> type_check(std::unique_ptr<resolved::Program> program, Symbols& symbols);

	private:

		Symbols& symbols;
	};
}