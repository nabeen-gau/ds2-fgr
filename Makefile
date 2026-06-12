CFLAGS := -ggdb -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CPPFLAGS := -Iother
DEPS := item_info.c other/miniz.c
SOURCES := main.c other/miniz.c

all: DS2FGR

item_info.c: data/item-lot.txt reformat-item-info.py
	python reformat-item-info.py

DS2FGR: $(DEPS)
	gcc $(CFLAGS) $(CPPFLAGS) -o DS2FGR $(SOURCES)

release: $(DEPS)
	gcc -O3 -Wno-unused-function -Wno-initializer-overrides $(CPPFLAGS) -o DS2FGR $(SOURCES)

memcheck: main.c
	clang -fsanitize=address -ggdb -Wall -Wextra -Wno-unused-function -Iother -o DS2FGR main.c other/miniz.c

test: test.c
	gcc -ggdb -Wall -Wextra -Iother -o test test.c other/miniz.c

dump-esd: dump-esd.c
	gcc -O3 $(CFLAGS) -o dump-esd dump-esd.c
