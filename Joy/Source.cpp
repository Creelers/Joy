#pragma warning(disable:4996)

#include "Common.h"
#include "Tokeniser.h"
#include "Ast.h"
#include "Parser.h"
#include "Symbol.h"
#include "ResolveSymbols.h"
#include "Compile.h"


using namespace syntax;

struct Position {
	u32 line;
	u32 column;
};

struct Span {
	Position start;
	Position end;
};

template<typename T> struct Spanned {
	Span span;
	T value;
};

int main(int argc, const char argv[]) {
	(void)argc;
	(void)argv;

	std::string src("proc main(argc: s32): s32 { return 0; }");
	
	Tokeniser tokeniser(src);
	Parser parser(tokeniser);
	auto tree = parser.parse();

	front::Symbols symbols;
	front::Resolver resolver(symbols);
	auto resolved = resolver.resolve_tree(tree);
	

	return 0;
}