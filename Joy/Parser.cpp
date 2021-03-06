#include "Parser.h"
#include "Tokeniser.h"
#include <variant>

namespace syntax {
	Parser::Parser(Tokeniser& tokeniser) : tokeniser(tokeniser) {}

	std::vector<std::unique_ptr<AstNode>> Parser::parse() {
		std::vector<std::unique_ptr<AstNode>> top_level;

		accept_token();
		while (!match({ TokenType::TOKEN_EOF })) {
			top_level.emplace_back(parse_top_level_stmt());
		}

		return top_level;
	}

	std::unique_ptr<AstNode> Parser::parse_top_level_stmt() {
		if (match({ TokenType::TOKEN_KEYWORD })) {
			if (std::get<std::string>(current_token.value) == "proc") {
				accept_token();
				return parse_proc_decl();
			}
		}
		else if (match({ TokenType::TOKEN_INT })) {
			return std::make_unique<AstNodeExprStmt>(parse_expr());
		}

		return nullptr;
	}

	std::unique_ptr<AstNodeProcDecl> Parser::parse_proc_decl() {
		auto name_tok = current_token;
		expect_token(TokenType::TOKEN_NAME);
		auto params = parse_params();
		expect_token(TokenType::TOKEN_RPAREN);
		std::unique_ptr<TypeSpec> ret_ty;
		if (match({ TokenType::TOKEN_COLON })) {
			accept_token();
			ret_ty = std::move(parse_type());
		}
		expect_token(TokenType::TOKEN_LBRACE);
		auto block = parse_block();
		expect_token(TokenType::TOKEN_RBRACE);

		return std::make_unique <AstNodeProcDecl>(AstNodeProcDecl(std::get<std::string>(name_tok.value), std::move(params), std::move(ret_ty), std::move(block)));
	}

	std::vector<std::unique_ptr<AstNodeProcParam>> Parser::parse_params() {
		expect_token(TokenType::TOKEN_LPAREN);
		std::vector<std::unique_ptr<AstNodeProcParam>> params;

		bool saw_comma = false;
		while (!match({ TokenType::TOKEN_RPAREN }) || saw_comma) {
			auto name_tok = current_token;
			expect_token(TokenType::TOKEN_NAME);
			expect_token(TokenType::TOKEN_COLON);
			auto ty = parse_type();
			params.emplace_back(std::make_unique<AstNodeProcParam>(std::get<std::string>(name_tok.value), std::move(ty)));
			if (match({ TokenType::TOKEN_COMMA })) {
				accept_token();
				saw_comma = true;
			}
			else {
				saw_comma = false;
			}
		}

		return params;
	}

	std::unique_ptr<AstNodeVarDecl> Parser::parse_var_decl() {
		auto name_tok = current_token;
		expect_token(TokenType::TOKEN_NAME);
		expect_token(TokenType::TOKEN_COLON);
		auto ty = parse_type();
		std::unique_ptr<AstNodeExpr> expr;
		if (match({ TokenType::TOKEN_ASSIGN })) {
			accept_token();
			expr = parse_expr();
		}

		return std::make_unique<AstNodeVarDecl>(std::get<std::string>(name_tok.value), std::move(ty), std::move(expr));
	}

	std::unique_ptr<AstNodePrintStmt> Parser::parse_print_stmt() {
		expect_token(TokenType::TOKEN_KEYWORD);
		if (!match({ TokenType::TOKEN_SEMICOLON })) {
			return std::make_unique<AstNodePrintStmt>(parse_expr());
		} else {
			return std::make_unique<AstNodePrintStmt>();
		}
	}

	std::unique_ptr<AstNodeReturnStmt> Parser::parse_return_stmt() {
		expect_token(TokenType::TOKEN_KEYWORD);

		return std::make_unique<AstNodeReturnStmt>(parse_expr());
	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr_operand() {
		if (match({ TokenType::TOKEN_INT })) {
			AstLiteralValue val;
			val.value = std::get<u64>(current_token.value);
			val.base = current_token.base;
			accept_token();
			return std::make_unique<AstNodeLitExpr>(val);
		}
		else if (match({ TokenType::TOKEN_NAME })) {
			AstLiteralValue val;
			val.value = std::get<std::string>(current_token.value);
			accept_token();
			return std::make_unique<AstNodeLitExpr>(val);
		}
		return nullptr;
	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr_base() {
		auto expr = parse_expr_operand();

		return expr;
	}

	UnaryExprKind Parser::token_to_unary() {
		if (match({ TokenType::TOKEN_SUB })) {
			return UnaryExprKind::Neg;
		}
		Rk_Assert(false);
		return UnaryExprKind::None;
	}

	BinaryExprKind Parser::token_to_binary() {
		if (match({ TokenType::TOKEN_SUB })) {
			return BinaryExprKind::Sub;
		} else if (match({ TokenType::TOKEN_ADD })) {
			return BinaryExprKind::Add;
		} else if (match({ TokenType::TOKEN_MUL })) {
			return BinaryExprKind::Mul;
		} else if (match({ TokenType::TOKEN_DIV })) {
			return BinaryExprKind::Div;
		}
		Rk_Assert(false);
		return BinaryExprKind::None;

	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr_unary() {
		if (match({ TokenType::TOKEN_SUB })) {
			auto op = token_to_unary();
			accept_token();
			return std::make_unique<AstNodeUnaryExpr>(op, std::move(parse_expr_operand()));
		}
		else {
			return parse_expr_base();
		}

		return nullptr;
	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr_add() {
		auto expr = parse_expr_mul();

		while (match({ TokenType::TOKEN_ADD, TokenType::TOKEN_SUB })) {
			auto op = token_to_binary();
			accept_token();
			expr = std::make_unique<AstNodeBinaryExpr>(op, std::move(expr), parse_expr_mul());
		}
		return expr;
	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr_mul() {
		auto expr = parse_expr_unary();

		while (match({ TokenType::TOKEN_MUL, TokenType::TOKEN_DIV })) {
			auto op = token_to_binary();
			accept_token();
			expr = std::make_unique<AstNodeBinaryExpr>(op, std::move(expr), parse_expr_unary());
		}

		return expr;
	}

	std::unique_ptr<AstNodeExpr> Parser::parse_expr() {
		return parse_expr_add();
	}

	std::vector<std::unique_ptr<AstNode>> Parser::parse_block() {
		std::vector<std::unique_ptr<AstNode>> statements;

		while (!match({ TokenType::TOKEN_RBRACE })) {
			if (match({ TokenType::TOKEN_NAME, TokenType::TOKEN_KEYWORD })) {
				if (std::get<std::string>(current_token.value) == "return") {
					statements.emplace_back(parse_return_stmt());
				} else if (std::get<std::string>(current_token.value) == "print") {
					statements.emplace_back(parse_print_stmt());
				}
				else {
					statements.emplace_back(parse_var_decl());
				}
			} else if (match({ TokenType::TOKEN_INT })) {
				statements.emplace_back(std::make_unique<AstNodeExprStmt>(parse_expr()));
			}
			expect_token(TokenType::TOKEN_SEMICOLON);
		}

		return statements;

	}

	std::unique_ptr<TypeSpec> Parser::parse_type() {
		if (match({ TokenType::TOKEN_NAME })) {
			TypeSpec ty;
			ty.name = std::get<std::string>(current_token.value);
			ty.kind = TypeSpecKind::Name;
			accept_token();
			return std::make_unique<TypeSpec>(std::move(ty));
		}
		else if (match({ TokenType::TOKEN_XOR })) {
			while (match({ TokenType::TOKEN_XOR })) {
				accept_token();
				auto ty = parse_type();
				TypeSpec ty_;
				ty_.kind = TypeSpecKind::Pointer;
				ty_.base = std::move(ty);
				return std::make_unique<TypeSpec>(std::move(ty_));
			}
		}
		return nullptr;
	}

	Token Parser::accept_token() {
		auto ret = current_token;
		current_token = tokeniser.next_token();
		return ret;
	}

	void Parser::expect_token(TokenType type) {
		if (current_token.kind != type) {
			Rk_Assert(false);
		}
		current_token = tokeniser.next_token();
	}

	bool Parser::match(std::initializer_list<TokenType> tys) {
		for (auto t : tys) {
			if (current_token.kind == t) {
				return true;
			}
		}
		return false;
	}
}