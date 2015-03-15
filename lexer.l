%{
#include <string>
#include "ast.h"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
#undef YY_DECL
#define YY_NO_UNISTD_H
#define YY_DECL_IS_OURS 1
#define YY_DECL int yylex (void)
#define MAX_STR_CONST 65535
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;
extern "C" int yywrap() {
	return 1;
}

// never interactive 
int isatty(int x){ return 0; }

#pragma warning( disable : 4996 )

%}

%option yylineno
%option interactive

binary   0[bB][01]+
%x str

%%

" "					    ;
"\t"	                ;
"\n"					;
"\r"					;
{binary}			    { yylval.c = 0; for (char *p=yytext+2;*p;++p) { yylval.c*=2; yylval.c+= *p-'0'; } return NUMBER; }
false					{ yylval.c = 0; return NUMBER; }
true					{ yylval.c = 1; return NUMBER; }
[0-9]+					yylval.c=atoi(yytext); return NUMBER;
"{"						return LEFT_CURLY_BRACE;
"}"						return RIGHT_CURLY_BRACE;
"("						return LEFT_BRACE;
")"						return RIGHT_BRACE;
","						return COMMA;

"++"					return INC;
"--"					return DEC;
"+"						return PLUS;
"-"						return MINUS;
"*"						return MUL;
"/"						return DIV;
"!"						return LNEG;
"~"						return BNEG;
"<<"					return SHIFTL;
">>"					return SHIFTR;
"<"						return LESS;
">"						return MORE;
"<="					return LESS_EQ;
">="					return MORE_EQ;
"&"						return BAND;
"|"						return BOR;
"^"						return BXOR;
"=="					return EQ;
"!="					return NEQ;
"||"					return OR;
"&&"					return AND;
"="						return SET;
"*="					return SET_MUL;
"/="					return SET_DIV;
"%="					return SET_MOD;
"+="					return SET_ADD;
"-="					return SET_SUB;
"<<="					return SET_SHIFT_L;
">>="					return SET_SHIFT_R;
"&="					return SET_AND;
"^="					return SET_XOR;
"|="					return SET_OR;

if						return IF;
for						return FOR;
else					return ELSE;
do						return DO;
while					return WHILE;
continue				return CONT;
break					return BREAK;
print					return PRINT;
scan					return SCAN;

";"						return EOS;

"/*"					{ char zz=0;
							for (;;) { char z=
#ifdef __cplusplus
							  yyinput();
#else
							  input();
#endif
							  if (z==EOF) { printf("EOF Error in comment"); return 0; }
							  if (zz=='*' && z=='/') break;
							  zz=z; } }
"//"				{ for (;;) { char z=
#ifdef __cplusplus
							  yyinput();
#else
							  input();
#endif
							  if (z==EOF) return 0;
							  if (z=='\n') { break; } } }

\"							  string_buf_ptr = string_buf; BEGIN(str);
     
							  <str>\"        { /* saw closing quote - all done */
								  BEGIN(INITIAL);
								  *string_buf_ptr = '\0';
								  yylval.string = new std::string(string_buf, strlen(string_buf));
								  return STRINGLIT;
								  }
     
							  <str>\n        {
								  /* error - unterminated string constant */
								  printf("Unterminated string literar."); return 0;
								  }
     
							  <str>\\[0-7]{1,3} {
								  /* octal escape sequence */
								  int result;
       
		  						  (void) sscanf( yytext + 1, "%o", &result );
     
								  if ( result > 0xff )
										  /* error, constant is out-of-bounds */
     
								  *string_buf_ptr++ = result;
								  }
     
							  <str>\\[0-9]+ {
								  {printf("Error bad escape"); return 0;}
								  }
     
							  <str>\\n  *string_buf_ptr++ = '\n';
							  <str>\\t  *string_buf_ptr++ = '\t';
							  <str>\\r  *string_buf_ptr++ = '\r';
							  <str>\\b  *string_buf_ptr++ = '\b';
							  <str>\\f  *string_buf_ptr++ = '\f';
     
							  <str>\\(.|\n)  *string_buf_ptr++ = yytext[1];
     
							  <str>[^\\\n\"]+        {
								  char *yptr = yytext;
       
		  						  while ( *yptr )
										  *string_buf_ptr++ = *yptr++;
								  }
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return IDENTIFIER; 
.                       printf("Unknown token! <"); printf(yytext); printf(">\n"); yyterminate();

%%