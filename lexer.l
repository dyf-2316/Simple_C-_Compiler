%{
#include <iostream>
#include "TreeNode.h"
#include "parser.tab.h"
// #define YYSTYPE TreeNode*
%}
%{
    int lines = 1, columns = 1;
%}

line \n

commentsbegin "/*" 
commentselement .|\n 
commentsend "*/"
commentbegin "//" 
commentelement .
commentend \n

%x COMMENTS
%x COMMENT

IF if
ELSE else
DO do
WHILE while
FOR for
VOID void
INT int
CHAR char
BOOLEAN bool
CONST const
RETURN return

ADD \+ 
MIN \-
MUL \*
DIV \/
MOD \%
DADD \+\+
DMIN \-\-

ASSIGN =

INPUT scanf
OUTPUT printf

MAIN main

EQU ==
GTR \>
LSS \< 
GEQ \>= 
LEQ \<=
NEQ !=

LB \{    
RB \}
LP \(
RP \) 
LBK \[
RBK \]

LOGICAL_AND &&
LOGICAL_OR \|\|
LOGICAL_NOT !

SEM ;
COM ,

NUM 0|([1-9][0-9]*)|(0[1-7][0-7]*)|(0[Xx][1-9a-fA-F][0-9a-fA-F]*)

B [ ]+
T [\t]+

ID [A-Za-z_][A-Za-z0-9_]*

STR (\"[^\"]*\")|(\'[^\']\')




%%

{line} {
    lines++; 
    columns = 1;  
}
<COMMENTS>{line} {
    lines++;
}


{commentsbegin} {BEGIN COMMENTS;} 
<COMMENTS>{commentselement} {} 
<COMMENTS>{commentsend} {columns = 1; BEGIN INITIAL;}

{commentbegin} {BEGIN COMMENT;} 
<COMMENT>{commentelement} {} 
<COMMENT>{commentend} {
    columns = 1; 
    lines++;
    BEGIN INITIAL;
    }

{IF} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return IF;
    }
{ELSE} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return ELSE;
    }
{WHILE} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return WHILE;
    }
{DO} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return DO;
    }
{FOR} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return FOR;
    }
{VOID} { 
    yylval.node = newTypeNode(Void);
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return VOID;
    }
{INT} { 
    yylval.node = newTypeNode(Integer);
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return INT;
    }
{CHAR} { 
    yylval.node = newTypeNode(Char);
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return CHAR;
    }
{BOOLEAN} { 
    yylval.node = newTypeNode(Boolean);
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return BOOLEAN;
    }
{CONST} { 
    yylval.node = newConstNode();
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return CONST;
    }
{RETURN} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return RETURN;
    }
{INPUT} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return INPUT; 
    }
{OUTPUT} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return OUTPUT; 
    }

{MAIN} { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return MAIN; 
    }

{ADD}  {  
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Add;
    return ADD;
    }
{MUL}  { 
    yylval.detail.pos = new Coordinate(lines, columns) ;
    columns += yyleng;
    yylval.detail.op = Mul;
    return MUL;
    }
{MIN}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Min;
    return MIN;
    }
{DIV}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Div;
    return DIV;
    }
{MOD}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Mod;
    return MOD;
    }
{DADD}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Dadd;
    return DADD;
    }
{DMIN}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Dmin;
    return DMIN;
    }
{ASSIGN}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Assign;
    return ASSIGN;
    }
{EQU}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Equ;
    return EQU;
    }
{GTR}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Gtr;
    return GTR;
    }
{LSS}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Lss;
    return LSS;
    }
{GEQ}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Geq;
    return GEQ;
    }
{LEQ}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Leq;
    return LEQ;
    }
{NEQ}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Neq;
    return NEQ;
    }
{LB}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return LB;
    } 
{RB}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return RB;
    }
{LP}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return LP;
    }
{RP}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return RP;
    }
{LBK}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return LBK;
    }
{RBK}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return RBK;
    }

{LOGICAL_AND}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Logical_and;
    return LOGICAL_AND;
    }
{LOGICAL_OR}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Logical_or;
    return LOGICAL_OR;
    }
{LOGICAL_NOT}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    yylval.detail.op = Logical_not;
    return LOGICAL_NOT;
    }
{SEM}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return SEM;
    }
{COM}  { 
    yylval.detail.pos = new Coordinate(lines, columns);
    columns += yyleng;
    return COM;
    }

{ID} { 
    yylval.node = newIdNode(strdup(yytext));
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return ID;   
    }
{NUM} { 
    yylval.node = newIntConstNode(strdup(yytext));
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return NUM;
    }
{STR} {
    yylval.node = newStrConstNode(strdup(yytext));
    yylval.node->pos = new Coordinate(lines, columns);
    columns += yyleng;
    return STR;
}

{B} {columns += yyleng;}
{T} {columns += yyleng*4;}

%%

int yywrap(void) { 
    return 1;
}


