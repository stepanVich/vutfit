CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic
all: interpret

interpret: ial.o lexikalni_analyzator.o syntax_expr.o syntax_ll.o interpret.o built_in_functions.o
	$(CC) ial.o lexikalni_analyzator.o syntax_expr.o syntax_ll.o interpret.o built_in_functions.o -o interpret

ial.o: ial.c ial.h err.h
	$(CC) $(CFLAGS) -c ial.c
lexikalni-analyzator.o: lexikalni_analyzator.c lexikalni_analyzator.h ial.h err.h
	$(CC) $(CFLAGS) -c lexikalni_analyzator.c
syntax-expr.o: syntax_expr.c syntax_expr.h syntax_ll.h interpret.h ial.h lexikalni_analyzator.h err.h
	$(CC) $(CFLAGS) -c syntax_expr.c
syntax-ll.o: syntax_ll.c syntax_ll.h syntax_expr.h interpret.h ial.h lexikalni_analyzator.h err.h
	$(CC) $(CFLAGS) -c syntax_ll.c
interpret.o: interpret.c interpret.h ial.h syntax_ll.h err.h
	$(CC) $(CFLAGS) -c interpret.c
built-in_functions.o: built_in_functions.c built_in_functions.h
	$(CC) $(CFLAGS) -c built_in_functions.c

test: run_tests.py
	python run_tests.py

clean:
	rm -f *.o interpret testy/*_test.stdout

tgz:
	tar -cvf xturec06.tgz built_in_functions.c built_in_functions.h err.h ial.c ial.h interpret.c interpret.h lexikalni_analyzator.c lexikalni_analyzator.h Makefile syntax_expr.c syntax_expr.h syntax_ll.c syntax_ll.h rozdeleni dokumentace.pdf

tgz_pokusne:
	tar -cvf xturec06.tgz testy built_in_functions.c built_in_functions.h err.h ial.c ial.h interpret.c interpret.h lexikalni_analyzator.c lexikalni_analyzator.h Makefile syntax_expr.c syntax_expr.h syntax_ll.c syntax_ll.h rozdeleni dokumentace.pdf run_tests.py
