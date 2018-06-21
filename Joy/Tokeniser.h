#pragma once

#include "Common.h"
#include <variant>

namespace syntax {
	inline bool is_keyword_name(std::string name) {
		return name == "if" || name == "else" || name == "while" ||
			name == "for" || name == "break" || name == "continue" ||
			name == "return" || name == "proc" || name == "struct" ||
			name == "tpedef" || name == "const" || name == "print";
	}

	inline u8 char_to_digit(u8 c) {
		switch (c) {
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a':case'A': return 10;
		case 'b':case'B': return 11;
		case 'c':case'C': return 12;
		case 'd':case'D': return 13;
		case 'e':case'E': return 14;
		case 'f':case'F': return 15;
		default: return 0;
		}
	}

	enum class TokenType {
		TOKEN_EOF,
		TOKEN_COLON,
		TOKEN_LPAREN,
		TOKEN_RPAREN,
		TOKEN_LBRACE,
		TOKEN_RBRACE,
		TOKEN_LBRACKET,
		TOKEN_RBRACKET,
		TOKEN_COMMA,
		TOKEN_DOT,
		TOKEN_AT,
		TOKEN_POUND,
		TOKEN_ELLIPSIS,
		TOKEN_QUESTION,
		TOKEN_SEMICOLON,
		TOKEN_KEYWORD,
		TOKEN_INT,
		TOKEN_FLOAT,
		TOKEN_STR,
		TOKEN_NAME,
		TOKEN_NEG,
		TOKEN_NOT,
		TOKEN_MUL,
		TOKEN_DIV,
		TOKEN_MOD,
		TOKEN_AND,
		TOKEN_LSHIFT,
		TOKEN_RSHIFT,
		TOKEN_ADD,
		TOKEN_SUB,
		TOKEN_XOR,
		TOKEN_OR,
		TOKEN_EQ,
		TOKEN_NOTEQ,
		TOKEN_LT,
		TOKEN_GT,
		TOKEN_LTEQ,
		TOKEN_GTEQ,
		TOKEN_AND_AND,
		TOKEN_OR_OR,
		TOKEN_ASSIGN,
		TOKEN_ADD_ASSIGN,
		TOKEN_SUB_ASSIGN,
		TOKEN_OR_ASSIGN,
		TOKEN_AND_ASSIGN,
		TOKEN_XOR_ASSIGN,
		TOKEN_LSHIFT_ASSIGN,
		TOKEN_RSHIFT_ASSIGN,
		TOKEN_MUL_ASSIGN,
		TOKEN_DIV_ASSIGN,
		TOKEN_MOD_ASSIGN,
		TOKEN_INC,
		TOKEN_DEC,
		TOKEN_COLON_ASSIGN,
		TOKEN_PRINT,
		NUM_TOKEN_KINDS,
	};

	enum class NumberMod {
		MOD_NONE,
		MOD_HEX,
	};

#define STRINGIFY( name ) #name

	static const char *token_kind_names[] = {
		STRINGIFY(TOKEN_EOF),
		STRINGIFY(TOKEN_COLON),
		STRINGIFY(TOKEN_LPAREN),
		STRINGIFY(TOKEN_RPAREN),
		STRINGIFY(TOKEN_LBRACE),
		STRINGIFY(TOKEN_RBRACE),
		STRINGIFY(TOKEN_LBRACKET),
		STRINGIFY(TOKEN_RBRACKET),
		STRINGIFY(TOKEN_COMMA),
		STRINGIFY(TOKEN_DOT),
		STRINGIFY(TOKEN_AT),
		STRINGIFY(TOKEN_POUND),
		STRINGIFY(TOKEN_ELLIPSIS),
		STRINGIFY(TOKEN_QUESTION),
		STRINGIFY(TOKEN_SEMICOLON),
		STRINGIFY(TOKEN_KEYWORD),
		STRINGIFY(TOKEN_INT),
		STRINGIFY(TOKEN_FLOAT),
		STRINGIFY(TOKEN_STR),
		STRINGIFY(TOKEN_NAME),
		STRINGIFY(TOKEN_NEG),
		STRINGIFY(TOKEN_NOT),
		STRINGIFY(TOKEN_MUL),
		STRINGIFY(TOKEN_DIV),
		STRINGIFY(TOKEN_MOD),
		STRINGIFY(TOKEN_AND),
		STRINGIFY(TOKEN_LSHIFT),
		STRINGIFY(TOKEN_RSHIFT),
		STRINGIFY(TOKEN_ADD),
		STRINGIFY(TOKEN_SUB),
		STRINGIFY(TOKEN_XOR),
		STRINGIFY(TOKEN_OR),
		STRINGIFY(TOKEN_EQ),
		STRINGIFY(TOKEN_NOTEQ),
		STRINGIFY(TOKEN_LT),
		STRINGIFY(TOKEN_GT),
		STRINGIFY(TOKEN_LTEQ),
		STRINGIFY(TOKEN_GTEQ),
		STRINGIFY(TOKEN_AND_AND),
		STRINGIFY(TOKEN_OR_OR),
		STRINGIFY(TOKEN_ASSIGN),
		STRINGIFY(TOKEN_ADD_ASSIGN),
		STRINGIFY(TOKEN_SUB_ASSIGN),
		STRINGIFY(TOKEN_OR_ASSIGN),
		STRINGIFY(TOKEN_AND_ASSIGN),
		STRINGIFY(TOKEN_XOR_ASSIGN),
		STRINGIFY(TOKEN_LSHIFT_ASSIGN),
		STRINGIFY(TOKEN_RSHIFT_ASSIGN),
		STRINGIFY(TOKEN_MUL_ASSIGN),
		STRINGIFY(TOKEN_DIV_ASSIGN),
		STRINGIFY(TOKEN_MOD_ASSIGN),
		STRINGIFY(TOKEN_INC),
		STRINGIFY(TOKEN_DEC),
		STRINGIFY(TOKEN_COLON_ASSIGN),
		STRINGIFY(TOKEN_PRINT),
	};

	static_assert((sizeof(token_kind_names) / sizeof(token_kind_names[0])) == to_underlying(TokenType::NUM_TOKEN_KINDS), "Not all tokens have a string representation");

#undef STRINGIFY

	struct Token {
		TokenType kind;
		NumberMod mod;
		s32 base;
		std::variant<u64, double, std::string>  value;
	};

	class Tokeniser {
	public:
		Tokeniser(const std::string& input);
		Tokeniser(const char *stream, usize stream_len);
		~Tokeniser() = default;
		Token next_token();

	private:
		const char *stream;
		const char *stream_end;
		u32 current_line;
		u32 current_column;
	};
}