WFLAGS := -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CFLAGS := -ggdb $(WFLAGS)
CPPFLAGS := -Iother
MINIZ := other/miniz.c
DEPS := item_info.c $(MINIZ)
SOURCES := main.c $(MINIZ)

all: ui

item_info.c: data/item-lot.txt reformat-item-info.py
	python reformat-item-info.py

resource.res: resource.rc
	windres resource.rc -O coff -o resource.res

DS2FGR: $(DEPS)
	gcc $(CFLAGS) $(CPPFLAGS) -o DS2FGR $(SOURCES)

pdb: $(DEPS)
	./llvm-mingw-20260616-msvcrt-x86_64/llvm-mingw-20260616-msvcrt-x86_64/bin/gcc.exe $(CFLAGS) $(CPPFLAGS) -Wl,--pdb=DS2FGR.pdb -o DS2FGR $(SOURCES)

test: test.c
	gcc -ggdb -Wall -Wextra -Iother -o test test.c other/miniz.c

dump-esd: dump-esd.c
	gcc -O3 $(CFLAGS) -o dump-esd dump-esd.c

map.svg: map.dot
	other\Graphviz-14.1.5-win64\bin\dot.exe -Tsvg map.dot -o map.svg

fgr: main.c resource.res item_info.c
	gcc -c main.c $(WFLAGS) $(CPPFLAGS) -o fgr.o

fgr-release: main.c resource.res item_info.c
	gcc -O3 -DRELEASE -c main.c $(WFLAGS) $(CPPFLAGS) -o fgr.o

ui: ui.c fgr
	gcc ui.c fgr.o $(MINIZ) resource.res -o DS2FGR -ldwmapi -lgdi32

release: ui.c fgr-release
	if not exist ".\bin" mkdir ".\bin"
	if exist ".\bin\mod" rmdir /s /q ".\bin\mod"
	mkdir ".\bin\mod"
	gcc -O3 ui.c fgr.o $(MINIZ) resource.res -o ".\bin\mod\DS2FGR" -ldwmapi -lgdi32
	xcopy /s /q /i /y "data" ".\bin\mod\data"

zip: release
	cd bin && zip -r DS2FGR.zip mod/DS2FGR.exe mod/data/enc_regulation.bnd.dcx mod/data/map/ mod/data/menu/ mod/data/Param/ mod/data/ezstate/ dinput8.dll modengine.ini

flo: flo.c common.c nob.h
	gcc $(CFLAGS) -o flo flo.c
