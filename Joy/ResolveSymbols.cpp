#include "ResolveSymbols.h"
#include <algorithm>

namespace front {
	std::unique_ptr<resolved::Program> Resolver::resolve_tree(std::vector<std::unique_ptr<syntax::AstNode>>& tree) {
		std::vector<std::unique_ptr<resolved::Proc>> procs;
		for (auto& node : tree) {
			syntax::AstNodeProcDecl *proc = reinterpret_cast<syntax::AstNodeProcDecl*>(node.get());
			if (proc != nullptr) {
				std::vector<std::string> param_names;
				std::transform(proc->params.begin(), proc->params.end(), std::back_inserter(param_names), [](const auto& p) -> auto { return p->name; });
				add_proc(proc->name, param_names);
				procs.emplace_back(std::move(resolve_proc(proc)));
			}
		}

		return std::make_unique<resolved::Program>(std::move(procs));
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

		scopes.push_back(std::map<std::string, Symbol>());
		auto params = resolve_params(proc->params);
		auto ret_ty = resolve_type(proc->ret);
				
		for (auto& p : params) {
			add_local(symbols.get_name(p->name), p->name);
		}

		std::vector<std::unique_ptr<resolved::Statement>> statements;
		std::transform(proc->statements.begin(), proc->statements.end(), std::back_inserter(statements), [this](auto& s) -> auto { return resolve_statement(s); });

		scopes.clear();

		return std::make_unique<resolved::Proc>(sym, std::move(params), std::move(ret_ty), std::move(statements));
	}

	std::vector<std::unique_ptr<resolved::ProcParam>> Resolver::resolve_params(std::vector<std::unique_ptr<syntax::AstNodeProcParam>>& params) {
		std::vector<std::unique_ptr<resolved::ProcParam>> resolved_params;
		for (auto& p : params) {
			auto symbol = symbols.new_symbol(p->name);
			auto ty = resolve_type(p->ty);
			if (find_local(p->name)) {
				Rk_Assert(false); //already declared
			}
			add_local(p->name, symbol);
			resolved_params.emplace_back(std::make_unique<resolved::ProcParam>(symbol, std::move(ty)));
		}

		return resolved_params;
	}

	std::unique_ptr<resolved::Statement> Resolver::resolve_statement(std::unique_ptr<syntax::AstNode>& statement) {
		switch (statement->type) {
		case syntax::NodeType::RetStmt: {
			auto node = dynamic_cast<syntax::AstNodeReturnStmt *>(statement.get());
			auto expr = resolve_expression(node->expr);
			resolved::Return ret;
			ret.expr = std::move(expr);
			resolved::Statement stmt;
			stmt.stmt = std::move(ret);
			return std::make_unique<resolved::Statement>(std::move(stmt));
		}
		case syntax::NodeType::PrintStmt: {
			auto node = dynamic_cast<syntax::AstNodePrintStmt *>(statement.get());
			resolved::Print print;
			if (node->expr != nullptr) {
				auto expr = resolve_expression(node->expr);
				print.expr = std::move(expr);
			}
			resolved::Statement stmt;
			stmt.stmt = std::move(print);
			return std::make_unique<resolved::Statement>(std::move(stmt));
		}
		case syntax::NodeType::ExprStmt: {
			auto node = dynamic_cast<syntax::AstNodeExprStmt *>(statement.get());
			auto expr = resolve_expression(node->expr);
			resolved::ExpressionStatement expr_stmt;
			expr_stmt.expr = std::move(expr);
			resolved::Statement stmt;
			stmt.stmt = std::move(expr_stmt);
			return std::make_unique<resolved::Statement>(std::move(stmt));
		}
		case syntax::NodeType::VarDecl: {
			auto node = dynamic_cast<syntax::AstNodeVarDecl *>(statement.get());
			resolved::VarDecl var_decl;
			var_decl.name = symbols.new_symbol(node->name);
			if (find_local(node->name)) {
				Rk_Assert(false); //declared already
			}
			add_local(node->name, var_decl.name);
			var_decl.ty = resolve_type(node->ty);
			resolved::Statement stmt;
			if (node->expr) {
				var_decl.expr = std::move(resolve_expression(node->expr));
			}
			stmt.stmt = std::move(var_decl);
			return std::make_unique<resolved::Statement>(std::move(stmt));
		}
		}

		//unexpected statement
		Rk_Assert(false);
		return nullptr;
	}

	struct LiteralVistor {
		LiteralVistor(Symbols& syms) : symbols(syms) {}
		resolved::literal_v operator()(const std::string& s) { resolved::literal_v ret; ret = symbols.new_symbol(s); return ret; }
		resolved::literal_v operator()(const u64 u) { resolved::literal_v ret; ret = u; return ret; }
		Symbols& symbols;
	};


	std::unique_ptr<resolved::Expression> Resolver::resolve_expression(std::unique_ptr<syntax::AstNodeExpr>& expr) {
		switch (expr->type) {
		case syntax::NodeType::LitExpr: {
			auto lit = dynamic_cast<syntax::AstNodeLitExpr *>(expr.get());
			auto lit_val = std::visit(LiteralVistor(symbols), lit->value.value);
			resolved::Expression ret;
			ret.expr = lit_val;
			return std::make_unique<resolved::Expression>(std::move(ret));
		}
		case syntax::NodeType::BinaryExpr: {
			auto node = dynamic_cast<syntax::AstNodeBinaryExpr *>(expr.get());
			auto resolved_left = resolve_expression(node->left);
			auto resolved_right = resolve_expression(node->right);
			resolved::BinaryExpression bin_exp;
			bin_exp.left = std::move(resolved_left);
			bin_exp.right = std::move(resolved_right);
			bin_exp.kind = node->kind;
			resolved::Expression expression;
			expression.expr = std::move(bin_exp);
			return std::make_unique<resolved::Expression>(std::move(expression));
		}
		}
		//unexpected expression
		Rk_Assert(false);
		return nullptr;
	}

	std::unique_ptr<resolved::TypeSpec> Resolver::resolve_type(std::unique_ptr<syntax::TypeSpec>& type) {
		if (type->kind == syntax::TypeSpecKind::Pointer) {
			auto ty = resolve_type(type->base);
			return std::make_unique<resolved::TypeSpec>(resolved::TypeSpecKind::Pointer, std::move(ty));
		}
		else if (type->kind == syntax::TypeSpecKind::Name) {
			if (type->name == "s32") {
				return std::make_unique<resolved::TypeSpec>(resolved::TypeSpecKind::S32);
			}
			else {
				Rk_Assert(false);//type not supported
				return nullptr; //NOTE: remove warning
			}
		}
		else {
			return nullptr;
		}
	}

	void Resolver::add_local(const std::string& name, Symbol symbol) {
		auto& scope = scopes.back();
		scope.insert(std::make_pair(name, symbol));
	}

	bool Resolver::find_local(const std::string& name) {
		auto& scope = scopes.back();
		return scope.count(name) == 1;
	}
}
