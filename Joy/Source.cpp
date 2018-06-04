#pragma warning(disable:4996)

#include "Common.h"
#include "Tokeniser.h"
#include "Ast.h"
#include "Parser.h"
#include "Symbol.h"
#include "Resolver.h"

using namespace syntax;

int main(int argc, const char argv[]) {
	(void)argc;
	(void)argv;

	std::string src("proc main(argc: s32, argv: ^^char): s32 { a: s32 = 0; return a; }");
	
	Tokeniser tokeniser(src);
	Parser parser(tokeniser);
	auto tree = parser.parse();

	front::Symbols symbols;
	front::Resolver resolver(symbols);
	resolver.resolve_tree(tree);

	return 0;
}