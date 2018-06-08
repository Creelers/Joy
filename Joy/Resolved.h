#pragma once

#include "Common.h"
#include "Ast.h"
#include "Symbol.h"
#include <map>

namespace front {
	namespace resolved {
		enum class TypeSpecKind {
			Pointer,
			S32,
		};

		struct TypeSpec {
			TypeSpecKind kind;
			std::unique_ptr<TypeSpec> base;

			TypeSpec() : base() {}
			TypeSpec(TypeSpecKind _kind) : kind(_kind), base() {}
			TypeSpec(TypeSpecKind _kind, std::unique_ptr<TypeSpec> _base) : kind(_kind), base(std::move(_base)) {}
		};

		struct Literal {
			syntax::AstLiteralValue expr;

			Literal() {}
			Literal(syntax::AstLiteralValue _expr) : expr(_expr) {}
		};

		struct Expression;
		struct ExpressionStatement {
			std::unique_ptr<Expression> expr;

			ExpressionStatement() : expr() {}
			ExpressionStatement(std::unique_ptr<Expression> _expr) : expr(std::move(_expr)) {}
		};

		struct BinaryExpression {
			syntax::BinaryExprKind kind;
			std::unique_ptr<Expression> left;
			std::unique_ptr<Expression> right;
		};

		struct Expression {
			std::variant<
				Literal, 
				ExpressionStatement,
				BinaryExpression
			> expr;
		};

		struct VarDecl {
			Symbol name;
			std::unique_ptr<TypeSpec> ty;
			std::unique_ptr<Expression> expr;
		};

		struct Return {
			std::unique_ptr<Expression> expr;
		};

		struct Print {
			std::unique_ptr<Expression> expr;
		};

		struct Statement {
			std::variant<
				VarDecl,
				Return,
				Print,
				ExpressionStatement
			> stmt;

			
		};

		struct ProcParam {
			Symbol name;
			std::unique_ptr<TypeSpec> ty;

			ProcParam(Symbol _name, std::unique_ptr<TypeSpec> _ty) : name(_name), ty(std::move(_ty)) {}
		};

		struct Proc {
			Symbol name;
			std::vector<std::unique_ptr<ProcParam>> params;
			std::unique_ptr<TypeSpec> ret_ty;
			std::vector<std::unique_ptr<Statement>> body;

			Proc() : params(), ret_ty(), body() {}
			Proc(Symbol _name, std::vector<std::unique_ptr<ProcParam>> _params, std::unique_ptr<TypeSpec> _ret_ty, std::vector<std::unique_ptr<Statement>> _body) : 
				name(_name), params(std::move(_params)), ret_ty(std::move(_ret_ty)), body(std::move(_body)) 
			{}
		};

		struct ResolvedAst {
			std::vector<std::unique_ptr<Proc>> procs;

			ResolvedAst() {}
			ResolvedAst(std::vector<std::unique_ptr<Proc>> _procs) : procs(std::move(_procs)) {}
		};
	}
}