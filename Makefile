TEST_DIR := ./src/tests
SRC := ./src

#test: tests.out
#	./tests.out

a.out: $(SRC)/main.c $(SRC)/parser.c $(SRC)/ast.c $(SRC)/lexer.c $(SRC)/string.c
	gcc -g $(SRC)/main.c $(SRC)/parser.c $(SRC)/ast.c $(SRC)/lexer.c $(SRC)/string.c -o $@


tests.out: $(TEST_DIR)/main.c $(SRC)/lexer.c $(SRC)/string.c
	gcc -g $(TEST_DIR)/main.c $(SRC)/lexer.c $(SRC)/string.c -o $@
