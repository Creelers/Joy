#pragma once
#include "Common.h"
#include <map>

namespace front {
	typedef u32 Symbol;

	struct Symbols {
		Symbol next_symbol;
		std::map<Symbol, std::string> symbols;

		Symbols() { next_symbol = 0; }

		Symbol new_symbol(const std::string& name) {
			auto symbol = next_symbol;
			next_symbol += 1;
			symbols.insert(std::make_pair(symbol, name));
			return symbol;
		}

		const std::string& get_name(Symbol symbol) {
			Rk_Assert(symbols.count(symbol) == 1);
			return symbols.find(symbol)->second;
		}
	};
}