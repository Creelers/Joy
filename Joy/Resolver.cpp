#include "Resolver.h"
#include <algorithm>

namespace front {
	resolved::ResolvedAst Resolver::resolve_tree(std::vector<std::unique_ptr<syntax::AstNode>>& tree) {
		collect_globals(tree);
		for (auto& node : tree) {
			syntax::AstNodeProcDecl *proc = reinterpret_cast<syntax::AstNodeProcDecl*>(node.get());
			if (proc != nullptr) {
				resolve_proc(proc);
			}

		}
			
		return {};
	}

	void Resolver::collect_globals(std::vector<std::unique_ptr<syntax::AstNode>>& tree) {
		for (auto& node : tree) {
			syntax::AstNodeProcDecl *proc = reinterpret_cast<syntax::AstNodeProcDecl*>(node.get());
			if (proc != nullptr) {
				std::vector<std::string> param_names;
				std::transform(proc->params.begin(), proc->params.end(), std::back_inserter(param_names), [](const auto& p) {return p->name; });
				add_proc(proc->name, param_names);
			}
		}
	}

	void Resolver::add_proc(std::string& name, std::vector<std::string>& param_names) {
		if (procs.count(name) == 1) {
			//already defined
			Rk_Assert(false);
		}

		auto symbol = symbols.new_symbol(name);
		procs.insert(std::make_pair(name, Proc(symbol, param_names)));
	}

	std::unique_ptr<resolved::Proc> Resolver::resolve_proc(syntax::AstNodeProcDecl *proc) {

	}

}