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
		};

		struct Expr {
			std::variant<
				std::unique_ptr<syntax::AstNodeLitExpr>
			> expr;
		};

		struct Statement {
			std::variant<
				std::unique_ptr<syntax::AstNodeReturnStmt>
			> stmt;
		};

		struct ProcParam {
			Symbol name;
			TypeSpec ty;
		};

		struct Proc {
			Symbol name;
			std::vector<std::unique_ptr<ProcParam>> params;
			TypeSpec ret_ty;
		};

		struct ResolvedAst {
			std::vector<std::unique_ptr<Proc>> procs;
		};
	}
}