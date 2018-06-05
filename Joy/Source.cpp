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

	std::string src("proc foo(): s32 { return 1; } proc main(): s32 { return 0; }");
	
	Tokeniser tokeniser(src);
	Parser parser(tokeniser);
	auto tree = parser.parse();

	front::Symbols symbols;
	front::Resolver resolver(symbols);
	auto resolved = resolver.resolve_tree(tree);

	return 0;
}