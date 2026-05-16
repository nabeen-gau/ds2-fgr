DS2FGR: main.c
	gcc -ggdb -Wall -Wextra -Wno-unused-function -Iother -o DS2FGR main.c other/miniz.c

memcheck: main.c
	clang -fsanitize=address -ggdb -Wall -Wextra -Wno-unused-function -o DS2FGR main.c
