result: parser.y lexer.l TreeNode.h Sym_table_map.h
	bison -d  parser.y
	flex lexer.l
	g++ parser.tab.c lex.yy.c TreeNode.cpp Sym_table_map.cpp
	cat input|./a.out 

clean:
	rm -f *.o parser.tab.c parser.tab.h lex.yy.c y.output y.tab.c y.tab.h
