#pragma once

#include "Common.h"
#include "Resolved.h"
#include "Symbol.h"
#include "Ast.h"
#include <map>

namespace resolved = front::resolved;

namespace front {
	class Resolver {
	public:
		Resolver(Symbols& syms) : symbols(syms) {}

		resolved::ResolvedAst resolve_tree(std::vector<std::unique_ptr<syntax::AstNode>>& _tree);

	private:

		struct Proc {
			Symbol name;
			std::vector<std::string> param_names;

			Proc(Symbol sym, std::vector<std::string> params) : name(sym), param_names(params) {}
		};

		void collect_globals(std::vector<std::unique_ptr<syntax::AstNode>>& tree);
		void add_proc(std::string& name, std::vector<std::string>& param_names);
		std::unique_ptr<resolved::Proc> resolve_proc(syntax::AstNodeProcDecl *proc);


		std::map<std::string, Proc> procs;
		std::vector<std::map<std::string, Symbol>> scopes;
		Symbols& symbols;
	};
}