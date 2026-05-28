CFLAGS := -ggdb -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CPPFLAGS := -Iother
SOURCES := main.c other/miniz.c

DS2FGR: $(SOURCES)
	gcc $(CFLAGS) $(CPPFLAGS) -o DS2FGR $(SOURCES)

memcheck: main.c
	clang -fsanitize=address -ggdb -Wall -Wextra -Wno-unused-function -Iother -o DS2FGR main.c other/miniz.c

test: test.c
	gcc -ggdb -Wall -Wextra -Iother -o test test.c other/miniz.c
