#include "Tokeniser.h"
namespace syntax {
	Tokeniser::Tokeniser(const std::string& input) {
		stream = input.c_str();
		stream_end = stream + input.length();
		current_line = 1;
		current_column = 1;
	}

	Tokeniser::Tokeniser(const char *stream, usize stream_len) {
		this->stream = stream;
		stream_end = stream + stream_len;
		current_line = 1;
		current_column = 1;
	}

#define CASE1(c, k) \
    case c: \
        token.kind = k; \
        stream++; \
		current_column++; \
		break;

#define CASE2(c1, k1, c2, k2) \
    case c1: \
        token.kind = k1; \
        stream++; \
		current_column++; \
		if(*stream == c2) {\
			token.kind = k2;\
			stream++;\
			current_column++;\
		}\
		break;

#define CASE3(c1, k1, c2, k2, c3, k3) \
    case c1: \
        token.kind = k1; \
        stream++; \
		current_column++; \
		if(*stream == c2) {\
			token.kind = k2;\
			stream++;\
			current_column++;\
		} else if (*stream == c3) {\
			token.kind = k3;\
			stream++;\
			current_column++;\
		}\
		break;

	Token Tokeniser::next_token() {
	repeat:
		Token token;
		const char *tok_start = stream;

		switch (*stream) {
		case ' ': case '\t': case '\r': case '\n':
			while (isspace(*stream)) {
				if (*stream == '\n') {
					current_line++;
					current_column = 1;
					stream++;
				}
				else {
					current_column++;
					stream++;
				}
				goto repeat;
			}

		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
			while (isdigit(*stream) || *stream == '_') {
				stream++;
				current_column++;
			}
			token.mod = NumberMod::MOD_NONE;
			s32 base = 10;
			if (*tok_start == '0') {
				tok_start++;
				if (tolower(*tok_start) == 'x') {
					token.mod = NumberMod::MOD_HEX;
					base = 16;
				}
			}
			u64 val = 0;
			for (;;) {
				if (*tok_start == '_') {
					tok_start++;
					current_column++;
					continue;
				}
				u8 digit = char_to_digit(*tok_start);
				if (digit == 0 && *tok_start != '0') {
					break;
				}
				if (digit >= base) {
					Rk_Assert(false); //wrong base
				}
				if (val > ((ULLONG_MAX - digit) / base)) {
					while (isdigit(*tok_start)) {
						tok_start++;
					}
					Rk_Assert(false);//overflow
				}
				val = val * base + digit;
				tok_start++;
			}
			token.kind = TokenType::TOKEN_INT;
			token.value = val;
			token.base = base;
		}
				  break;

		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case '_':
			while (isalnum(*stream) || *stream == '_') {
				stream++;
				current_column++;
			}
			token.value = std::string(tok_start, stream - tok_start);
			token.kind = is_keyword_name(std::get<std::string>(token.value)) ? TokenType::TOKEN_KEYWORD : TokenType::TOKEN_NAME;
			break;

			CASE1('\0', TokenType::TOKEN_EOF)
				CASE1('(', TokenType::TOKEN_LPAREN)
				CASE1(')', TokenType::TOKEN_RPAREN)
				CASE1('{', TokenType::TOKEN_LBRACE)
				CASE1('}', TokenType::TOKEN_RBRACE)
				CASE1('[', TokenType::TOKEN_LBRACKET)
				CASE1(']', TokenType::TOKEN_RBRACKET)
				CASE1(',', TokenType::TOKEN_COMMA)
				CASE1('@', TokenType::TOKEN_AT)
				CASE1('#', TokenType::TOKEN_POUND)
				CASE1('?', TokenType::TOKEN_QUESTION)
				CASE1(';', TokenType::TOKEN_SEMICOLON)
				CASE1('~', TokenType::TOKEN_NEG)
				CASE1('/', TokenType::TOKEN_DIV);
				CASE2('!', TokenType::TOKEN_NOT, '=', TokenType::TOKEN_NOTEQ)
				CASE2(':', TokenType::TOKEN_COLON, '=', TokenType::TOKEN_COLON_ASSIGN)
				CASE2('=', TokenType::TOKEN_ASSIGN, '=', TokenType::TOKEN_EQ)
				CASE2('^', TokenType::TOKEN_XOR, '=', TokenType::TOKEN_XOR_ASSIGN)
				CASE2('*', TokenType::TOKEN_MUL, '=', TokenType::TOKEN_MUL_ASSIGN)
				CASE2('%', TokenType::TOKEN_MOD, '=', TokenType::TOKEN_MOD_ASSIGN)
				CASE3('+', TokenType::TOKEN_ADD, '=', TokenType::TOKEN_ADD_ASSIGN, '+', TokenType::TOKEN_INC)
				CASE3('-', TokenType::TOKEN_SUB, '=', TokenType::TOKEN_SUB_ASSIGN, '-', TokenType::TOKEN_DEC)
				CASE3('&', TokenType::TOKEN_AND, '=', TokenType::TOKEN_AND_ASSIGN, '&', TokenType::TOKEN_AND_AND)
				CASE3('|', TokenType::TOKEN_OR, '=', TokenType::TOKEN_OR_ASSIGN, '|', TokenType::TOKEN_OR_OR)
		}
		return token;
	}

#undef CASE1
#undef CASE2
#undef CASE3
}