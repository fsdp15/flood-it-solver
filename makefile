all:
	gcc main.c --std=c99 -o floodit -l cgraph
clean:
	rm floodit
