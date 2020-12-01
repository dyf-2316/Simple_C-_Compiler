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

INPUT input 
OUTPUT output 

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

WS [\t\r ]+

ID [A-Za-z_][A-Za-z0-9_]*

STR (\"[^\"]*\")|(\'[^\']\')




%%

{line} {
    lines++;   
}
<COMMENTS>{line} {
    lines++;
}


{commentsbegin} {BEGIN COMMENTS;} 
<COMMENTS>{commentselement} {} 
<COMMENTS>{commentsend} {BEGIN INITIAL;}

{commentbegin} {BEGIN COMMENT;} 
<COMMENT>{commentelement} {} 
<COMMENT>{commentend} {
    lines++;
    BEGIN INITIAL;
    }

{IF} { 
    return IF;
    }
{ELSE} { 
    return ELSE;
    }
{WHILE} { 
    return WHILE;
    }
{FOR} { 
    return FOR;
    }
{VOID} { 
    yylval.node = newTypeNode(Void);
    return VOID;
    }
{INT} { 
    yylval.node = newTypeNode(Integer);
    return INT;
    }
{CHAR} { 
    yylval.node = newTypeNode(Char);
    return CHAR;
    }
{BOOLEAN} { 
    yylval.node = newTypeNode(Boolean);
    return BOOLEAN;
    }
{CONST} { 
    return CONST;
    }
{RETURN} { 
    return RETURN;
    }
{INPUT} { 
    return INPUT; 
    }
{OUTPUT} { 
    return OUTPUT; 
    }

{MAIN} {
    return MAIN; 
    }

{ADD}  { 
    yylval.op = Add;
    return ADD;
    }
{MUL}  { 
    yylval.op = Mul;
    return MUL;
    }
{MIN}  { 
    yylval.op = Min;
    return MIN;
    }
{DIV}  { 
    yylval.op = Div;
    return DIV;
    }
{MOD}  { 
    yylval.op = Mod;
    return MOD;
    }
{DADD}  { 
    yylval.op = Dadd;
    return DADD;
    }
{DMIN}  { 
    yylval.op = Dmin;
    return DMIN;
    }
{ASSIGN}  { 
    yylval.op = Assign;
    return ASSIGN;
    }
{EQU}  { 
    yylval.op = Equ;
    return EQU;
    }
{GTR}  { 
    yylval.op = Gtr;
    return GTR;
    }
{LSS}  { 
    yylval.op = Lss;
    return LSS;
    }
{GEQ}  { 
    yylval.op = Geq;
    return GEQ;
    }
{LEQ}  { 
    yylval.op = Leq;
    return LEQ;
    }
{NEQ}  { 
    yylval.op = Neq;
    return NEQ;
    }
{LB}  { 
    return LB;
    } 
{RB}  { 
    return RB;
    }
{LP}  { 
    return LP;
    }
{RP}  { 
    return RP;
    }
{LBK}  { 
    return LBK;
    }
{RBK}  { 
    return RBK;
    }

{LOGICAL_AND}  { 
    yylval.op = Logical_and;
    return LOGICAL_AND;
    }
{LOGICAL_OR}  { 
    yylval.op = Logical_or;
    return LOGICAL_OR;
    }
{LOGICAL_NOT}  { 
    yylval.op = Logical_not;
    return LOGICAL_NOT;
    }
{SEM}  { 
    return SEM;
    }
{COM}  { 
    return COM;
    }

{ID} { 
    yylval.node = newIdNode(strdup(yytext));
    return ID;   
    }
{NUM} { 
    yylval.node = newIntConstNode(strdup(yytext));
    return NUM;
    }
{STR} {
    yylval.node = newStrConstNode(strdup(yytext));
    return STR;
}

{WS} {}

%%

int yywrap(void) { 
    return 1;
}


