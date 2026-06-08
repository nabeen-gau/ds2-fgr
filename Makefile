CFLAGS := -ggdb -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CPPFLAGS := -Iother
DEPS := data/item-lot-formatted.txt other/miniz.c
SOURCES := main.c other/miniz.c

all: DS2FGR

data/item-lot-formatted.txt: data/item-lot.txt
	python reformat-item-info.py

DS2FGR: $(DEPS)
	gcc $(CFLAGS) $(CPPFLAGS) -o DS2FGR $(SOURCES)

memcheck: main.c
	clang -fsanitize=address -ggdb -Wall -Wextra -Wno-unused-function -Iother -o DS2FGR main.c other/miniz.c

test: test.c
	gcc -ggdb -Wall -Wextra -Iother -o test test.c other/miniz.c

dump-esd: dump-esd.c
	gcc -O3 $(CFLAGS) -o dump-esd dump-esd.c
