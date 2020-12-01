%{
#include <stdio.h>
#include <stdlib.h>
#include "TreeNode.h"
void yyerror(const char* msg) {}
int yylex();
%}
%union{
  TreeNode* node;
  OpType op;
};

%token <node> NUM ID STR
%token <node> INT CHAR BOOLEAN VOID 
%token <op> ADD MIN MUL DIV MOD DADD DMIN ASSIGN EQU 
%token <op> GTR LSS GEQ LEQ NEQ LOGICAL_AND LOGICAL_OR LOGICAL_NOT UMIN UADD
%token <op> SEM LB RB COM LP RP LBK RBK
%token <op> FOR INPUT OUTPUT ELSE DO MAIN IF WHILE CONST RETURN
%token <node> EOL

%type <node> show_stmt expr stmt for_stmt com_stmt output_stmt input_stmt while_stmt stmts  
%type <node> if_else_stmt if_stmt else_stmt
%type <node> type_specifiers declaration init_declarator_list init_declarator declarator initializer

%left COM
%right ASSIGN
%left LOGICAL_OR
%left LOGICAL_AND
%left EQU NEQ
%left GTR LSS GEQ LEQ
%left ADD MIN
%left MUL DIV MOD
%right LOGICAL_NOT
%right UMIN UADD
%left  DADD DMIN 
%%

show_stmt : stmt           { $$ = $1; Operate($$); }

stmt    :  MAIN LP RP com_stmt { $$ = $4; }
		|  com_stmt        { $$ = $1; }
		|  output_stmt     { $$ = $1; }
		|  input_stmt      { $$ = $1; }
		|  while_stmt      { $$ = $1; }
		|  if_else_stmt    { $$ = $1; }
		|  if_stmt         { $$ = $1; }
		|  for_stmt        { $$ = $1; }
		|  expr  SEM       { $$ = $1; }
		|	declaration SEM		{ $$ = $1; }
		|	SEM             	{ $$ = NULL; }
		;

com_stmt : LB stmts RB {$$ = newComStmtNode($2); }
		 ;

stmts   :  stmt stmts 
			{ if($$ == NULL) {
					$$ = $2;
				}
				else {
					$1->sibling = $2; $$ = $1; 
				}
			}
        |  stmt { $$ = $1; }
		;

output_stmt  :  OUTPUT LP expr RP SEM{ $$ = newOutputStmtNode($3); }
			 ;

input_stmt :  INPUT LP expr RP SEM { $$ = newInputStmtNode($3); }
		   ;

while_stmt :  WHILE LP expr RP stmt { $$ = newWhileStmtNode(WhileK, $3, $5); }
           |  DO com_stmt WHILE LP expr RP SEM { $$ = newWhileStmtNode(DoWhileK, $2, $5); }
		   ;

for_stmt   :  FOR LP expr SEM expr SEM expr RP stmt { $$ = newForStmtNode($3, $5, $7, $9);}
		   |  FOR LP SEM expr SEM expr RP stmt { $$ = newForStmtNode(NULL, $4, $6, $8); }
		   |  FOR LP expr SEM SEM expr RP stmt { $$ = newForStmtNode($3, NULL, $6, $8); }
		   |  FOR LP expr SEM expr SEM RP stmt { $$ = newForStmtNode($3, $5, NULL, $8); }
		   |  FOR LP SEM SEM expr RP stmt { $$ = newForStmtNode(NULL, NULL, $5, $7); }
		   |  FOR LP SEM expr SEM RP stmt { $$ = newForStmtNode(NULL, $4, NULL, $7); }
		   |  FOR LP expr SEM SEM RP stmt { $$ = newForStmtNode($3, NULL, NULL, $7); }
		   |  FOR LP SEM SEM RP stmt { $$ = newForStmtNode(NULL, NULL, NULL, $6); }
		   ;

if_else_stmt : if_stmt else_stmt { $$ = newIfElseStmtNode($1, $2); }
			 ;

if_stmt :   IF LP expr RP stmt  { $$ = newIfStmtNode($3, $5); }
		;

else_stmt     :  ELSE stmt { $$ = newElseStmtNode($2); } 
              ; 

expr	:	expr ADD expr	{ $$ = newExpNode($2, $1, $3); }
		|	expr MIN expr	{ $$ = newExpNode($2, $1, $3); }
		|	expr MUL expr	{ $$ = newExpNode($2, $1, $3); }
		|	expr DIV expr	{ $$ = newExpNode($2, $1, $3); }
        | 	expr MOD expr	{ $$ = newExpNode($2, $1, $3); }
        |   expr DMIN       { $$ = newExpNode($2, $1, NULL); }
        |   expr DADD       { $$ = newExpNode($2, $1, NULL); }
        |   expr ASSIGN expr   { $$ = newExpNode($2, $1, $3); }
        |   expr EQU expr   { $$ = newExpNode($2, $1, $3); }
        |   expr GTR expr   { $$ = newExpNode($2, $1, $3); }
        |   expr LSS expr   { $$ = newExpNode($2, $1, $3); }
        |   expr GEQ expr   { $$ = newExpNode($2, $1, $3); }
        |   expr LEQ expr   { $$ = newExpNode($2, $1, $3); }
        |   expr NEQ expr   { $$ = newExpNode($2, $1, $3); }
        |   expr LOGICAL_AND expr  { $$ = newExpNode($2, $1, $3); }
        |   expr LOGICAL_OR expr   { $$ = newExpNode($2, $1, $3); }
        |   LOGICAL_NOT expr   { $$ = newExpNode($1, $2, NULL); }
        |   MIN expr %prec UMIN   { $$ = newExpNode($1, $2, NULL); }
        |   ADD expr %prec UADD   { $$ = newExpNode($1, $2, NULL); }
		|	LP expr RP	    { $$ = $2; }
		|	NUM             { $$ = $1; }   // $$=$1 can be ignored
        |   STR             { $$ = $1; }
		|   ID              { $$ = $1; }
		;


type_specifiers 		:  INT       { $$ = $1; }
						|  CHAR      { $$ = $1; }
						|  BOOLEAN   { $$ = $1; }
						|  VOID      { $$ = $1; }
						;

declaration    			:   type_specifiers init_declarator_list		{ $$ = newDeclNode($1, $2); }
			   			;

init_declarator_list	:	init_declarator								{ $$ = $1; }
						|	init_declarator_list COM init_declarator 	{ $1 -> sibling = $3; $$ = $1; }
						;

init_declarator			:	declarator									{ $$ = $1; }
						;

declarator				:	ID											{ $$ = $1; }
						;

initializer				:	expr										{ $$ = $1; }
						;



%%

int yylex();

int main() {
	ConstructMap();
    return yyparse();
}