CFLAGS := -ggdb -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CPPFLAGS := -Iother
DEPS := item_info.c other/miniz.c
SOURCES := main.c other/miniz.c

all: DS2FGR

item_info.c: data/item-lot.txt reformat-item-info.py
	python reformat-item-info.py

DS2FGR: $(DEPS)
	gcc $(CFLAGS) $(CPPFLAGS) -o DS2FGR $(SOURCES)

pdb: $(DEPS)
	./llvm-mingw-20260616-msvcrt-x86_64/llvm-mingw-20260616-msvcrt-x86_64/bin/gcc.exe $(CFLAGS) $(CPPFLAGS) -Wl,--pdb=DS2FGR.pdb -o DS2FGR $(SOURCES)

release: $(DEPS)
	gcc -O3 -Wno-unused-function -Wno-initializer-overrides $(CPPFLAGS) -o DS2FGR $(SOURCES)

test: test.c
	gcc -ggdb -Wall -Wextra -Iother -o test test.c other/miniz.c

dump-esd: dump-esd.c
	gcc -O3 $(CFLAGS) -o dump-esd dump-esd.c

map.svg: map.dot
	other\Graphviz-14.1.5-win64\bin\dot.exe -Tsvg map.dot -o map.svg
