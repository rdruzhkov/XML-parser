
OBJECTS= C_str.o C_Attributes.o Utility.o  y.tab.o lex.yy.o XML-parser.o

all: y.tab.c lex.yy.c $(OBJECTS) 
	gcc  $(OBJECTS) -lstdc++ -o XML-parser.out
	cp XML-parser.out ../Tests/XML-parser.out
	
.c.o:
	gcc -c -Wall $< -o $@	

.cpp.o:
	gcc -c -Wall -lstdc++ $< -o $@

lex.yy.c:
	flex -l lexems.l

y.tab.c:
	bison -y -d grammar.y

clean:
	rm -rf y.tab.c lex.yy.c y.tab.h
	rm -rf *.o
