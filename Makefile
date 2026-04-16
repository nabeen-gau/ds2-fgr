DS2FGR: main.c
	gcc -ggdb -Wall -Wextra -Wno-unused-function -o DS2FGR main.c

memcheck: main.c
	clang -fsanitize=address -ggdb -Wall -Wextra -Wno-unused-function -o DS2FGR main.c
