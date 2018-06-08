#pragma once

#include "Common.h"
#include "Resolved.h"

namespace vm {

	enum class Instruction : u8 {
		Add,
		Add1,
		Sub,
		Sub1,
		Mul,
		Div,
		Push,
		Pop,
		Print,
		Ret,
	};

	template<typename T, usize N = 2048>
	class Stack {
	public:
		Stack() : cursor(0) {}

		void push(const T& value) {
			if (top == N) {
				Rk_Assert(false);
			}
			stack[cursor++] = value;
		}

		T pop() {
			return stack[--top];
		}

	private:
		usize cursor;
		std::array<T, N> stack;
	};

	class VirtualMachine {
	public:


	private:
		Stack<u64> operand_stack;
	};

}