#pragma once

#include "Common.h"
#include "Resolved.h"

namespace vm {

	enum class Instruction : u8 {
		Push,
		Pop,
		Ret,
		Print,
	};

}