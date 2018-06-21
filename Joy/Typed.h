#pragma once

#include "Common.h"
#include "Symbol.h"
#include "Resolved.h"
#include <variant>

namespace front {
	namespace typed {
		struct LiteralExpr {
			resolved::literal_v lit;

			LiteralExpr() {}
			LiteralExpr(resolved::literal_v _lit) : lit(std::move(_lit)) {}
		};

		enum class Signedness {
			Signed,
			Unsigned,
		};

		enum class BitSize {
			B8,
			B16,
			B32,
			B64,
		};

		struct TypeInt {
			Signedness sign;
			BitSize size;
		};

		typedef std::variant<TypeInt> type_v;
		struct Type {
			type_v type;

			Type() {} 
			Type(type_v _type) : type(std::move(_type)) {}
		};

		typedef std::variant<LiteralExpr> expr_v;
		struct Expr {
			expr_v expr;

			Expr() {}
			Expr(expr_v _expr) : expr(std::move(_expr)) {}
		};


		struct TypedExpr {
			std::unique_ptr<Expr> expr;
			std::unique_ptr<Type> type;

			TypedExpr() : expr(), type() {}
			TypedExpr(std::unique_ptr<Expr> _expr, std::unique_ptr<Type> _type) : expr(std::move(_expr)), type(std::move(_type)) {}
		};

		struct RetStatement {
			std::unique_ptr<TypedExpr> expr;

			RetStatement() : expr() {}
			RetStatement(std::unique_ptr<TypedExpr> _expr) : expr(std::move(_expr)) {}
		};

		typedef std::variant<RetStatement> statement_v;
		struct Statement {
			statement_v statement;
		};

		struct ProcedureParam {
			Symbol name;
			std::unique_ptr<Type> ty;
		};

		struct Procedure {
			Symbol name;
			std::vector<ProcedureParam> params;
			std::unique_ptr<Type> ret_ty;
			std::vector<std::unique_ptr<Statement>> body;
		};

		struct Program {
			std::vector<Procedure> procedures;
		};
	}
}