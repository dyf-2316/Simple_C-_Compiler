result: parser.y lexer.l TreeNode.h
	bison -d  parser.y
	flex lexer.l
	g++ parser.tab.c lex.yy.c TreeNode.cpp 
	cat input|./a.out 

clean:
	rm -f *.o parser.tab.c parser.tab.h lex.yy.c y.output y.tab.c y.tab.h
