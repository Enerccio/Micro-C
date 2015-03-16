%{
	#define YY_NO_UNISTD_H
	#define YYERROR_VERBOSE
	#include "ast.h"

	Block* program;

	extern int yylex();
	extern int yylineno;
    
    void yyerror(const char *s) { printf("ERROR at line %i: %s\n", yylineno, s); }
%}

%debug
%locations

%union {
	Node* node;
	Block* block;
	Expression* expr;
	Statement* stmt;
	Int* intValue;
	Identifier* ident;
	Operation* op;
	UnaryOperation* unop;
	BinaryOperation* bop;
	IfStatement* ifs;
	ForStatement* fors;
	DoStatement* dos;
	WhileStatement* whs;
	ContinueStatement* conts;
	BreakStatement* breaks;
	PrintExpressionStatement* printes;
	PrintStringStatement* printss;
	PrintFormattedStatement* printfs;
	ScanStatement* scans;
	EmptyStatement* emptys;

	std::string* string;
	int c;
	int token;
}

/* Terminals */

%token <string> STRINGLIT IDENTIFIER
%token <c>      NUMBER
%token <token>  LEFT_CURLY_BRACE RIGHT_CURLY_BRACE LEFT_BRACE RIGHT_BRACE
%token <token>  INC DEC PLUS MINUS MUL DIV LNEG BNEG SHIFTL SHIFTR LESS MORE LESS_EQ MORE_EQ BAND BOR OR AND MOD
%token <token>  BXOR EQ NEQ SET_ADD SET SET_MUL SET_DIV SET_MOD SET_SUB SET_SHIFT_L SET_SHIFT_R SET_AND SET_XOR SET_OR
%token <token>  IF FOR ELSE DO WHILE CONT BREAK PRINT SCAN EOS COMMA

/* Nonterminals */

%type <block> program stmts block
%type <stmt>  stmt 
%type <expr>  expression identifier set_expr empty_expr for_first for_second for_third

/* Priority */

%left SET_ADD SET SET_MUL SET_DIV SET_MOD SET_SUB SET_SHIFT_L SET_SHIFT_R SET_AND SET_XOR SET_OR
%left OR
%left AND
%left BOR
%left BXOR
%left BAND
%left EQ NEQ 
%left LESS MORE LESS_EQ MORE_EQ
%left SHIFTL SHIFTR
%left PLUS MINUS 
%left MUL DIV MOD 
%left LNEG BNEG
%left INC DEC
%right ELSE "then" 

%start program

%%

program : block { program = $1; }
		;

block : LEFT_CURLY_BRACE stmts RIGHT_CURLY_BRACE { $$ = $2; }
      | LEFT_CURLY_BRACE RIGHT_CURLY_BRACE { $$ = new Block(); }
	  ;

stmts : stmt { $$ = new Block(); $$->statements.push_back($<stmt>1); }
	  | stmts stmt { $1->statements.push_back($<stmt>2); }
	  ;

stmt : block { $$ = $<block>1; }
	 | expression EOS { $$ = $<expr>1; $<expr>1->isStatement = true; }
	 | set_expr EOS { $$ = $<expr>1; }
	 | identifier SET_MUL expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_MUL, true); }
	 | identifier SET_DIV expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_DIV, true); }
	 | identifier SET_MOD expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_MOD, true); }
	 | identifier SET_ADD expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_ADD, true); }
	 | identifier SET_SUB expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_SUB, true); }
	 | identifier SET_SHIFT_L expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_SHIFT_L, true); }
	 | identifier SET_SHIFT_R expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_SHIFT_R, true); }
	 | identifier SET_AND expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_AND, true); }
	 | identifier SET_XOR expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_XOR, true); }
	 | identifier SET_OR expression EOS { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SET_OR, true); }
	 | empty_expr { $$ = $<expr>1; }
	 | IF LEFT_BRACE expression RIGHT_BRACE stmt %prec "then" {$$ = new IfStatement($<expr>3, $<stmt>5);}						
	 | IF LEFT_BRACE expression RIGHT_BRACE stmt ELSE stmt {$$ = new IfStatement($<expr>3, $<stmt>5, $<stmt>7);}
	 | FOR LEFT_BRACE for_first EOS for_second EOS for_third RIGHT_BRACE stmt { $$ = new ForStatement($<expr>3, $<expr>5, $<expr>7, $<stmt>9);}
	 | DO expression WHILE stmt {$$ = new DoStatement($<expr>2, $<stmt>4); }
	 | WHILE LEFT_BRACE expression RIGHT_BRACE stmt {$$ = new DoStatement($<expr>3, $<stmt>5); }
	 | CONT EOS { $$ = new ContinueStatement(); }
	 | BREAK EOS { $$ = new BreakStatement(); }
	 | PRINT LEFT_BRACE expression RIGHT_BRACE EOS { $$ = new PrintExpressionStatement($<expr>3); }
	 | PRINT LEFT_BRACE STRINGLIT RIGHT_BRACE EOS { $$ = new PrintStringStatement(std::string($3->c_str())); delete $3; }
	 | PRINT LEFT_BRACE STRINGLIT COMMA expression RIGHT_BRACE EOS { $$ = new PrintFormattedStatement(std::string($3->c_str()), $<expr>5); delete $3; }
	 | SCAN LEFT_BRACE identifier RIGHT_BRACE EOS { $$ = new ScanStatement($<ident>3); }
	 ;

set_expr : identifier SET expression {$$ = new BinaryOperation($<expr>1, $<expr>3, O_SET, true); }
		 ;

empty_expr : EOS {$$ = new EmptyStatement(); }
		   ;

for_first : empty_expr { $$ = $<expr>1; }
		  | set_expr { $$ = $<expr>1; }
		  ;

for_second : { $$ = new EmptyStatement(); }
		   | expression { $$ = $<expr>1; }
		   ;

for_third : { $$ = new EmptyStatement(); }
	      | expression { $$ = $<expr>1; }
		  ;

expression : NUMBER { $$ = new Int($1); }
		   | identifier { $$ = $<expr>1; }
		   | INC identifier { $$ = new UnaryOperation($<expr>2, O_ADD_BEFORE); }
		   | identifier INC { $$ = new UnaryOperation($<expr>1, O_ADD_AFTER); }
		   | DEC identifier { $$ = new UnaryOperation($<expr>2, O_SUB_BEFORE); }
		   | identifier DEC { $$ = new UnaryOperation($<expr>1, O_SUB_AFTER); }
		   | PLUS expression { $$ = $<expr>1; }
		   | MINUS expression { $$ = new UnaryOperation($<expr>2, O_UNARY_MINUS); }
		   | LNEG expression { $$ = new UnaryOperation($<expr>2, O_LOG_NEG); }
		   | BNEG expression { $$ = new UnaryOperation($<expr>2, O_BIT_NEG); }
		   | expression PLUS expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_PLUS); }
		   | expression MINUS expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_MINUS); }
		   | expression MUL expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_MUL); }
		   | expression DIV expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_DIV); }
		   | expression SHIFTL expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SHIFT_LEFT); }
		   | expression SHIFTR expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_SHIFT_RIGHT); }
		   | expression LESS expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_LESS); }
		   | expression MORE expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_MORE); }
		   | expression LESS_EQ expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_LESS_EQ); }
		   | expression MORE_EQ expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_MORE_EQ); }
		   | expression BAND expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_BIT_AND); }
		   | expression BOR expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_BIT_OR); }
		   | expression BXOR expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_BIT_XOR); }
		   | expression EQ expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_EQUALS); }
		   | expression NEQ expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_NEQUALS); }
		   | expression OR expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_LOG_OR); }
		   | expression AND expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_LOG_AND); }
		   | expression MOD expression { $$ = new BinaryOperation($<expr>1, $<expr>3, O_MOD); }
		   | LEFT_BRACE set_expr RIGHT_BRACE { $$ = $<expr>1; }
		   | LEFT_BRACE identifier SET_MUL expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_MUL); }
		   | LEFT_BRACE identifier SET_DIV expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_DIV); }
		   | LEFT_BRACE identifier SET_MOD expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_MOD); }
		   | LEFT_BRACE identifier SET_ADD expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_ADD); }
		   | LEFT_BRACE identifier SET_SUB expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_SUB); }
		   | LEFT_BRACE identifier SET_SHIFT_L expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_SHIFT_L); }
		   | LEFT_BRACE identifier SET_SHIFT_R expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_SHIFT_R); }
		   | LEFT_BRACE identifier SET_AND expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_AND); }
		   | LEFT_BRACE identifier SET_XOR expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_XOR); }
		   | LEFT_BRACE identifier SET_OR expression RIGHT_BRACE { $$ = new BinaryOperation($<expr>2, $<expr>4, O_SET_OR); }
		   | LEFT_BRACE expression RIGHT_BRACE { $$ = $2; }
		   ;

identifier : IDENTIFIER { $$ = new Identifier(std::string($1->c_str())); delete $1; }
		   ;
