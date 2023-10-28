all: src/main.c
	gcc $^ -o ./cpad -lncurses
