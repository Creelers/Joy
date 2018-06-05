#include "Resolver.h"
#include <algorithm>

namespace front {
	std::unique_ptr<resolved::ResolvedAst> Resolver::resolve_tree(std::vector<std::unique_ptr<syntax::AstNode>>& tree) {
		std::vector<std::unique_ptr<resolved::Proc>> procs;
		collect_globals(tree);
		for (auto& node : tree) {
			syntax::AstNodeProcDecl *proc = reinterpret_cast<syntax::AstNodeProcDecl*>(node.get());
			if (proc != nullptr) {
				procs.emplace_back(std::move(resolve_proc(proc)));
			}

		}

		return std::make_unique<resolved::ResolvedAst>(std::move(procs));
	}

	void Resolver::collect_globals(std::vector<std::unique_ptr<syntax::AstNode>>& tree) {
		for (auto& node : tree) {
			syntax::AstNodeProcDecl *proc = reinterpret_cast<syntax::AstNodeProcDecl*>(node.get());
			if (proc != nullptr) {
				std::vector<std::string> param_names;
				std::transform(proc->params.begin(), proc->params.end(), std::back_inserter(param_names), [](const auto& p) -> auto {return p->name; });
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
		auto sym = procs.find(proc->name)->second.name;
		auto params = resolve_params(proc->params);
		auto ret_ty = resolve_type(proc->ret);

		std::vector<std::unique_ptr<resolved::Statement>> statements;
		std::transform(proc->statements.begin(), proc->statements.end(), std::back_inserter(statements), [this](auto& s) -> auto {return resolve_statement(s); });

		return std::make_unique<resolved::Proc>(sym, std::move(params), std::move(ret_ty), std::move(statements));
	}

	std::vector<std::unique_ptr<resolved::ProcParam>> Resolver::resolve_params(std::vector<std::unique_ptr<syntax::AstNodeProcParam>>& params) {
		std::vector<std::unique_ptr<resolved::ProcParam>> resolved_params;
		for (auto& p : params) {
			auto symbol = symbols.new_symbol(p->name);
			auto ty = resolve_type(p->ty);
			resolved_params.emplace_back(std::make_unique<resolved::ProcParam>(symbol, std::move(ty)));
		}

		return resolved_params;
	}

	std::unique_ptr<resolved::Statement> Resolver::resolve_statement(std::unique_ptr<syntax::AstNode>& statement) {
		auto ret = dynamic_cast<syntax::AstNodeReturnStmt *>(statement.get());
		if (ret != nullptr) {
			auto expr = resolve_expression(ret->expr);
			resolved::Return ret;
			ret.expr = std::move(expr);
			resolved::Statement stmt;
			stmt.stmt = std::move(ret);
			return std::make_unique<resolved::Statement>(std::move(stmt));
		}
		//unexpected statement
		Rk_Assert(false);
		return nullptr;
	}

	std::unique_ptr<resolved::Expression> Resolver::resolve_expression(std::unique_ptr<syntax::AstNodeExpr>& expr) {
		auto lit = dynamic_cast<syntax::AstNodeLitExpr *>(expr.get());
		if (lit != nullptr) {
			auto lit_val = resolved::Literal(lit->value);
			resolved::Expression ret;
			ret.expr = lit_val;
			return std::make_unique<resolved::Expression>(std::move(ret));
		}
		//unexpect expression
		Rk_Assert(false);
		return nullptr;
	}
	
	std::unique_ptr<resolved::TypeSpec> Resolver::resolve_type(std::unique_ptr<syntax::TypeSpec>& type) {
		if (type->kind == syntax::TypeSpecKind::Pointer) {
			auto ty = resolve_type(type->base);
			return std::make_unique<resolved::TypeSpec>(resolved::TypeSpecKind::Pointer, std::move(ty));
		} else if (type->kind == syntax::TypeSpecKind::Name) {
			if (type->name == "s32") {
				return std::make_unique<resolved::TypeSpec>(resolved::TypeSpecKind::S32);
			} else {
				Rk_Assert(false);//type not supported
				return nullptr; //NOTE: remove warning
			}
		} else {
			return nullptr;
		}
	}
}