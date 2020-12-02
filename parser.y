%{
#include <stdio.h>
#include <stdlib.h>
#include "TreeNode.h"
void yyerror(const char* msg) {}
int yylex();
%}
%union{
  TreeNode* node;
  union {OpType op; Coordinate* pos;} detail;
};

%token <node> NUM ID STR
%token <node> INT CHAR BOOLEAN VOID 
%token <detail> ADD MIN MUL DIV MOD DADD DMIN ASSIGN EQU 
%token <detail> GTR LSS GEQ LEQ NEQ LOGICAL_AND LOGICAL_OR LOGICAL_NOT UMIN UADD
%token <detail> SEM LB RB COM LP RP LBK RBK
%token <detail> FOR INPUT OUTPUT ELSE DO MAIN IF WHILE RETURN
%token <node> CONST

%type <node> expression expr 
%type <node> if_stmt else_stmt
%type <node> type_specifiers declaration init_declarator_list init_declarator declarator initializer const_specifiers declaration_specifier
%type <node> statement compound_statement iteration_statement selection_statement IO_statement expression_statement
%type <node> block_item_list block_item
%type <node> program 

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

program 				:  block_item_list       							{ $$ = newProgramNode($1);$$->pos = $1->pos ; Operate($$); }

block_item_list   		:  block_item block_item_list {
							if($$ == NULL) {
								$$ = $2;
							}
							else {
								$1->sibling = $2; $$ = $1; 
								}
							}
        				|  block_item { $$ = $1; }
						;

block_item				:	declaration	SEM									{ $$ = $1; }
						| 	statement										{ $$ = $1; }
						;

statement   			:  	MAIN LP RP compound_statement 					{ $$ = $4; }
						|  	compound_statement        						{ $$ = $1; }
						|  	IO_statement    								{ $$ = $1; }
						|	selection_statement								{ $$ = $1; }
						|  	iteration_statement								{ $$ = $1; }
						|  	expression_statement  							{ $$ = $1; }
						;

compound_statement 		: 	LB block_item_list RB 							{$$ = newComStmtNode($2); $$->pos = $1.pos;$$->attr.val = (void*) $3.pos;}
		 				;

IO_statement			:	INPUT LP expr RP SEM 							{ $$ = newInputStmtNode($3); $$->pos = $1.pos;}
						|	OUTPUT LP expr RP SEM							{ $$ = newOutputStmtNode($3); $$->pos = $1.pos;}
						;

if_stmt 				:   IF LP expr RP statement  						{ $$ = newIfStmtNode($3, $5); $$->pos = $1.pos;}
						;

else_stmt     			:  ELSE statement 									{ $$ = newElseStmtNode($2); $$->pos = $1.pos;} 
              			; 

selection_statement		:  	if_stmt else_stmt 								{ $$ = newIfElseStmtNode($1, $2); $$->pos = $1->pos;}
						|  	if_stmt         								{ $$ = $1; }
						;	

iteration_statement		:  	FOR LP expression SEM expression SEM expression RP statement 	{ $$ = newForStmtNode($3, $5, $7, $9);$$->pos = $1.pos;}
						|  	FOR LP declaration SEM expression SEM expression RP statement 	{ $$ = newForStmtNode($3, $5, $7, $9);$$->pos = $1.pos;}
						|	WHILE LP expression RP statement 								{ $$ = newWhileStmtNode(WhileK, $3, $5); $$->pos = $1.pos;}
						|	DO compound_statement WHILE LP expression RP SEM 				{ $$ = newWhileStmtNode(DoWhileK, $2, $5); $$->pos = $1.pos;}
		   				;

expression_statement	:	expression SEM									{ $$ = $1;}

expression				:	expr COM expression 							{ $1 -> sibling = $3; $$ = $1;ShowNode($$);}
						|	expr											{ $$ = $1;}
						| 													{ $$ = NULL;}
						;

expr					:	expr ADD expr									{ $$ = newExpNode($2.op, $1, $3); }
						|	expr MIN expr									{ $$ = newExpNode($2.op, $1, $3); }
						|	expr MUL expr									{ $$ = newExpNode($2.op, $1, $3); }
						|	expr DIV expr									{ $$ = newExpNode($2.op, $1, $3); }
        				| 	expr MOD expr									{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr DMIN       								{ $$ = newExpNode($2.op, $1, NULL); }
        				|   expr DADD       								{ $$ = newExpNode($2.op, $1, NULL); }
        				|   expr ASSIGN expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr EQU expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr GTR expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr LSS expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr GEQ expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr LEQ expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr NEQ expr   								{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr LOGICAL_AND expr  							{ $$ = newExpNode($2.op, $1, $3); }
        				|   expr LOGICAL_OR expr   							{ $$ = newExpNode($2.op, $1, $3); }
        				|   LOGICAL_NOT expr   								{ $$ = newExpNode($1.op, $2, NULL); }
        				|   MIN expr %prec UMIN   							{ $$ = newExpNode($1.op, $2, NULL); }
        				|   ADD expr %prec UADD   							{ $$ = newExpNode($1.op, $2, NULL); }
						|	LP expr RP	    								{ $$ = $2; }
						|	NUM             								{ $$ = $1; }   // $$=$1 can be ignored
        				|   STR             								{ $$ = $1; }
						|   ID              								{ $$ = $1; }
						;

declaration    			:   declaration_specifier init_declarator_list 		{ $$ = newDeclNode($1, $2); $$->pos = $1->pos;}
			   			;

declaration_specifier   :	type_specifiers const_specifiers				{ $$ = newDeclSpecNode($1, $2); $$->pos = $1->pos;}
						|	const_specifiers type_specifiers				{ $$ = newDeclSpecNode($1, $2); $$->pos = $1->pos;}
						|	type_specifiers									{ $$ = newDeclSpecNode($1, NULL); $$->pos = $1->pos;}
						;

type_specifiers 		:  	INT       										{ $$ = $1; }
						| 	CHAR      										{ $$ = $1; }
						|  	BOOLEAN   										{ $$ = $1; }
						|  	VOID      										{ $$ = $1; }
						;

const_specifiers		:	CONST											{ $$ = $1; }

init_declarator_list	:	init_declarator									{ $$ = $1;}
						|	init_declarator COM init_declarator_list		{ $1 -> sibling = $3; $$ = $1;}
						;

init_declarator			:	declarator										{ $$ = $1; }
						|	declarator ASSIGN initializer					{ $$ = newInitNode($1, $3); $$->pos = $1->pos;}
						;

declarator				:	ID												{ $$ = $1; }
						;

initializer				:	expr											{ $$ = $1; }
						;



%%

int yylex();

int main() {
	ConstructMap();
    return yyparse();
}