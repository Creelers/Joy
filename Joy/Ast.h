#pragma once

#include "Common.h"
#include "Tokeniser.h"

namespace syntax {
	enum class TypeSpecKind {
		Name,
		Pointer,
	};

	struct TypeSpec {
		TypeSpecKind kind;
		std::unique_ptr<TypeSpec> base;
		std::string name;
	};

	enum class NodeType {
		VarDecl,
		ProcParam,
		ProcDecl,
		RetStmt,
		PrintStmt,

		Expr,
		LitExpr,
		UnaryExpr,
		BinaryExpr,
		ExprStmt,
	};

	struct AstNode {
		NodeType type;
		virtual ~AstNode() = default;
	};

	struct AstNodeExpr;

	struct AstNodeVarDecl : public AstNode {
		static const NodeType n_type = NodeType::VarDecl;
		std::string name;
		std::unique_ptr<TypeSpec> ty;
		std::unique_ptr<AstNodeExpr> expr;

		AstNodeVarDecl(std::string _name, std::unique_ptr<TypeSpec> _ty, std::unique_ptr<AstNodeExpr> _expr) : name(_name), ty(std::move(_ty)), expr(std::move(_expr))
		{
			type = n_type;
		}
	};

	struct AstNodeProcParam : public AstNode {
		static const NodeType n_type = NodeType::ProcParam;
		std::string name;
		std::unique_ptr<TypeSpec> ty;
		AstNodeProcParam(std::string _name, std::unique_ptr<TypeSpec> _ty) : name(_name), ty(std::move(_ty)) { type = n_type; }
	};

	struct AstNodeProcDecl : public AstNode {
		static const NodeType n_type = NodeType::ProcDecl;
		std::string name;
		std::vector<std::unique_ptr<AstNodeProcParam>> params;
		std::unique_ptr<TypeSpec> ret;
		std::vector<std::unique_ptr<AstNode>> statements;
		AstNodeProcDecl(std::string _name, std::vector<std::unique_ptr<AstNodeProcParam>> _params, std::unique_ptr<TypeSpec> _ret, std::vector<std::unique_ptr<AstNode>> _statements)
			: name(_name), params(std::move(_params)), ret(std::move(_ret)), statements(std::move(_statements))
		{
			type = n_type;
		}
	};

	struct AstNodeReturnStmt : public AstNode {
		static const NodeType n_type = NodeType::RetStmt;
		std::unique_ptr<AstNodeExpr> expr;

		AstNodeReturnStmt(std::unique_ptr<AstNodeExpr> _expr) : expr(std::move(_expr))
		{
			type = n_type;
		}
	};

	struct AstNodePrintStmt : public AstNode {
		static const NodeType n_type = NodeType::PrintStmt;
		std::unique_ptr<AstNodeExpr> expr;
		AstNodePrintStmt() : expr() 
		{
			type = n_type;
		}

		AstNodePrintStmt(std::unique_ptr<AstNodeExpr> _expr) : expr(std::move(_expr))
		{
			type = n_type;
		}
	};

	struct AstNodeExpr : public AstNode {
		static const NodeType n_type = NodeType::Expr;
	};

	enum class LiteralKind {
		Int,
		Float,
		Bool,
		String,
	};

	struct AstLiteralValue {
		s32 base;
		std::variant<u64, std::string> value;
	};

	struct AstNodeLitExpr : public AstNodeExpr {
		static const NodeType n_type = NodeType::LitExpr;
		AstLiteralValue value;
		AstNodeLitExpr(AstLiteralValue _value) : value(_value)
		{
			type = n_type;
		}
		AstNodeLitExpr(NumberMod _mod, AstLiteralValue _value) : value(_value)
		{
			type = n_type;
		}
	};

	enum class UnaryExprKind {
		None,
		Neg,
	};

	struct AstNodeUnaryExpr : public AstNodeExpr {
		static const NodeType n_type = NodeType::UnaryExpr;
		UnaryExprKind kind;
		std::unique_ptr<AstNodeExpr> expr;

		AstNodeUnaryExpr(UnaryExprKind _kind, std::unique_ptr<AstNodeExpr> _expr) : kind(_kind), expr(std::move(_expr))
		{
			type = n_type;
		}
	};

	enum class BinaryExprKind {
		None,
		Add,
		Sub,
		Mul,
		Div,
	};

	struct AstNodeBinaryExpr : public AstNodeExpr {
		static const NodeType n_type = NodeType::BinaryExpr;
		BinaryExprKind kind;
		std::unique_ptr<AstNodeExpr> left;
		std::unique_ptr<AstNodeExpr> right;

		AstNodeBinaryExpr(BinaryExprKind _kind, std::unique_ptr<AstNodeExpr> _left, std::unique_ptr<AstNodeExpr> _right) : kind(_kind), left(std::move(_left)), right(std::move(_right))
		{
			type = n_type;
		}
	};

	struct AstNodeExprStmt : public AstNode {
		static const NodeType n_type = NodeType::ExprStmt;
		std::unique_ptr<AstNodeExpr> expr;
		AstNodeExprStmt(std::unique_ptr<AstNodeExpr> _expr) : expr(std::move(_expr)) { type = n_type; }
	};

	template<typename T>
	T MakeNode() {
		T node;
		node.type = T::n_type;
		return node;
	}
}