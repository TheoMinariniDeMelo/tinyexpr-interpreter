TEST_DIR := ./src/tests
SRC := ./src

#test: tests.out
#	./tests.out

tests.out: $(TEST_DIR)/main.c $(SRC)/lexer.c $(SRC)/string.c
	gcc -g $(TEST_DIR)/main.c $(SRC)/lexer.c $(SRC)/string.c -o $@