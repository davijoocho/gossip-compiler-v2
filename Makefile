dcc: program.o lexer.o parser.o
	gcc -o dcc program.o lexer.o parser.o

program.o: ./src/program.c
	gcc -c -I./include ./src/program.c

lexer.o: ./src/lexer.c ./include/lexer.h
	gcc -c -I./include ./src/lexer.c

parser.o: ./src/parser.c ./include/parser.h
	gcc -c -I./include ./src/parser.c

clean:
	rm *.exe *.o dcc

