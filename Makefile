sudoku: sudoku.c
	gcc -o sudoku -Wall -O3 sudoku.c

.PHONY: clean

clean:
	rm sudoku

