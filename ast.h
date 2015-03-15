#pragma once

#include <iostream>
#include <vector>
#include <string>

class Statement;
class Expression;

typedef std::vector<Statement*> StatementList;
typedef std::vector<Expression*> ExpressionList;

enum NodeType
{
	BLOCK, NUMBER_EXP, IDENTIFIER_EXP, UNARY_OPERATION, BINARY_OPERATION,
	IF_STATEMENT, FOR_STATEMENT, DO_STATEMENT, WHILE_STATEMENT, CONTINUE_STATEMENT, BREAK_STATEMENT, PRINT_EXPR_STATEMENT, PRINT_STRING_STATEMENT, PRINT_FORMAT_STATEMENT, SCAN_STATEMENT, EMPTY_STATEMENT
};

class Node
{
public:
	virtual ~Node(){}
	virtual NodeType GetType() = 0;
};

class Statement  : public Node
{
};

class Block : public Statement
{
public:
	StatementList statements;
	virtual NodeType GetType(){ return BLOCK; }
};

class Expression : public Statement 
{
public:
	Expression(){ isStatement = false; }
	bool isStatement;
};

class Int : public Expression
{
public:
	int value;
	Int(int value) : value(value){}
	virtual NodeType GetType(){ return NUMBER_EXP; }
};

class Identifier : public Expression
{
public:
	std::string ident;
	Identifier(std::string id) : ident(id) {}
	virtual NodeType GetType(){ return IDENTIFIER_EXP; }
};

class Operation : public Expression
{

};

enum OperationType
{
	O_ADD_BEFORE, O_ADD_AFTER, O_SUB_BEFORE, O_SUB_AFTER, O_UNARY_MINUS, O_LOG_NEG, O_BIT_NEG, O_SET,

	O_MUL, O_DIV, O_MOD, O_PLUS, O_MINUS, O_SHIFT_LEFT, O_SHIFT_RIGHT,
	O_EQUALS, O_NEQUALS, O_LESS, O_MORE, O_LESS_EQ, O_MORE_EQ,

	O_BIT_OR, O_BIT_XOR, O_BIT_AND, O_LOG_AND, O_LOG_OR,

	O_SET_MUL, O_SET_DIV, O_SET_MOD, O_SET_ADD, O_SET_SUB, O_SET_SHIFT_L, O_SET_SHIFT_R, O_SET_AND, O_SET_XOR, O_SET_OR
};

class UnaryOperation : public Expression
{
public:
	Expression* expression;
	OperationType type;
	UnaryOperation(Expression* e, OperationType type) : expression(e), type(type) {}
	virtual ~UnaryOperation(){ delete expression; }
	virtual NodeType GetType(){ return UNARY_OPERATION; }
};

class BinaryOperation : public Expression
{
public:
	Expression* expression1;
	Expression* expression2;
	OperationType type;
	BinaryOperation(Expression* e1, Expression* e2, OperationType type) : expression1(e1), expression2(e2), type(type) { isStatement = false; }
	BinaryOperation(Expression* e1, Expression* e2, OperationType type, bool is) : expression1(e1), expression2(e2), type(type) { isStatement = is; }
	virtual ~BinaryOperation(){ delete expression1; delete expression2; }
	virtual NodeType GetType(){ return BINARY_OPERATION; }
};

class IfStatement : public Statement
{
public:
	Expression* test;
	Statement* exec;
	Statement* fail;

	IfStatement(Expression* t, Statement* e) : test(t), exec(e), fail(NULL) {}
	IfStatement(Expression* t, Statement* e, Statement* f) : test(t), exec(e), fail(f) {}
	virtual ~IfStatement(){ delete test;  delete exec; delete fail; }
	virtual NodeType GetType(){ return IF_STATEMENT; }
};

class ForStatement : public Statement
{
public:
	Expression* init;
	Expression* cycle;
	Expression* test;

	Statement* exec;

	ForStatement(Expression* i, Expression* c, Expression* t, Statement* e) :
		init(i), cycle(c), test(t), exec(e) {}
	virtual ~ForStatement(){ delete init; delete cycle; delete test; delete exec; }
	virtual NodeType GetType(){ return FOR_STATEMENT; }
};

class DoStatement : public Statement
{
public:
	Expression* test;
	Statement* exec;

	DoStatement(Expression* t, Statement* e) : test(t), exec(e) {}
	virtual ~DoStatement(){ delete test; delete exec; }
	virtual NodeType GetType(){ return DO_STATEMENT; }
};

class WhileStatement : public Statement
{
public:
	Expression* test;
	Statement* exec;

	WhileStatement(Expression* t, Statement* e) : test(t), exec(e) {}
	virtual ~WhileStatement(){ delete test; delete exec; }
	virtual NodeType GetType(){ return WHILE_STATEMENT; }
};

class ContinueStatement : public Statement
{
public:
	virtual NodeType GetType(){ return CONTINUE_STATEMENT; }
};

class BreakStatement : public Statement
{
public:
	virtual NodeType GetType(){ return BREAK_STATEMENT; }
};

class PrintExpressionStatement : public Statement
{
public:
	Expression* expression;
	PrintExpressionStatement(Expression *e) : expression(e) {}
	virtual ~PrintExpressionStatement(){ delete expression; }
	virtual NodeType GetType(){ return PRINT_EXPR_STATEMENT; }
};

class PrintStringStatement : public Statement
{
public:
	std::string print;
	PrintStringStatement(std::string p) : print(p) {}
	virtual NodeType GetType(){ return PRINT_STRING_STATEMENT; }
};

class PrintFormattedStatement : public Statement
{
public:
	std::string format;
	Expression* expression;
	PrintFormattedStatement(std::string f, Expression* e) : format(f), expression(e) {}
	virtual ~PrintFormattedStatement(){ delete expression; }
	virtual NodeType GetType(){ return PRINT_FORMAT_STATEMENT; }
};

class ScanStatement : public Statement
{
public:
	Identifier* ident;
	ScanStatement(Identifier* i) : ident(i) {}
	virtual ~ScanStatement(){ delete ident; }
	virtual NodeType GetType(){ return SCAN_STATEMENT; }
};

class EmptyStatement : public Statement
{
public:
	virtual NodeType GetType(){ return EMPTY_STATEMENT; }
};