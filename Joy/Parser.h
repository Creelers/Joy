#pragma once

#include "Ast.h"
#include "Common.h"
#include "Tokeniser.h"

namespace syntax {
	class Parser {
	public:
		Parser(Tokeniser& tokeniser);

		std::vector<std::unique_ptr<AstNode>> parse();

	private:
		Tokeniser & tokeniser;

		std::unique_ptr<AstNode> parse_top_level_stmt();
		std::unique_ptr<AstNodeProcDecl> parse_proc_decl();
		std::vector<std::unique_ptr<AstNodeProcParam>> parse_params();
		std::vector<std::unique_ptr<AstNode>> parse_block();
		std::unique_ptr<AstNodeVarDecl> parse_var_decl();
		std::unique_ptr<AstNodeReturnStmt> parse_return_stmt();
		std::unique_ptr<AstNodeExpr> parse_expr_operand();
		std::unique_ptr<AstNodeExpr> parse_expr_base();
		std::unique_ptr<AstNodeExpr> parse_unary_expr();
		std::unique_ptr<AstNodeExpr> parse_expr();

		std::unique_ptr<TypeSpec> parse_type();

		UnaryExprKind token_to_unary();

		Token accept_token();
		void expect_token(TokenType type);
		bool match(std::initializer_list<TokenType> tys);


		Token current_token;
	};
}