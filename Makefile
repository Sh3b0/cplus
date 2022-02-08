all:
	flex -o lexer.cpp lexer.l
	bison -d -o parser.cpp parser.y 
	g++ -g main.cpp lexer.cpp parser.cpp shell.cpp -std=c++17 -o cplus

debug:
	flex -o lexer.cpp lexer.l
	bison --debug --report=all -g -d -o parser.cpp parser.y
	g++ -g main.cpp lexer.cpp parser.cpp shell.cpp -std=c++17 -o cplus

clean:
	rm -rf lexer.cpp parser.cpp parser.hpp cplus parser.dot parser.output build
