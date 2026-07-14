CC := gcc
# CC := ./tools/llvm-mingw-20260616-msvcrt-x86_64/llvm-mingw-20260616-msvcrt-x86_64/bin/gcc.exe

WFLAGS := -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CFLAGS := -ggdb $(WFLAGS)
CPPFLAGS := -Iother
MINIZ := other/miniz.c
DEPS := item_info.c $(MINIZ)
SOURCES := main.c $(MINIZ)
MAIN_OBJ := fgr.o
OUT_NAME := DS2FGR
OUT_EXE := $(OUT_NAME).exe
ICON_RES := resource.res

ZIPPED_FILES := mod/$(OUT_EXE) mod/data/enc_regulation.bnd.dcx mod/data/map/ mod/data/menu/ mod/data/Param/ mod/data/ezstate/ dinput8.dll modengine.ini

all: ui

item_info.c: data/item-lot.txt reformat-item-info.py
	python reformat-item-info.py

resource.res: resource.rc
	windres resource.rc -O coff -o $(ICON_RES)

DS2FGR: $(DEPS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(OUT_NAME) $(SOURCES)

pdb: $(DEPS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -Wl,--pdb=$(OUT_NAME).pdb -o $(OUT_NAME) $(SOURCES)

test: test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test test.c $(MINIZ)

dump-esd: dump-esd.c
	$(CC) -O3 $(CFLAGS) -o dump-esd dump-esd.c

map.svg: map.dot
	other\Graphviz-14.1.5-win64\bin\dot.exe -Tsvg map.dot -o map.svg

fgr: main.c $(ICON_RES) item_info.c
	$(CC) -c main.c $(WFLAGS) $(CPPFLAGS) -o $(MAIN_OBJ)

fgr-release: main.c $(ICON_RES) item_info.c
	$(CC) -O3 -DRELEASE -c main.c $(WFLAGS) $(CPPFLAGS) -o $(MAIN_OBJ)

ui: ui.c fgr
	$(CC) ui.c $(MAIN_OBJ) $(MINIZ) $(ICON_RES) -o $(OUT_NAME) -ldwmapi -lgdi32

release: ui.c fgr-release
	if not exist ".\bin" mkdir ".\bin"
	if exist ".\bin\mod" rmdir /s /q ".\bin\mod"
	mkdir ".\bin\mod"
	$(CC) -O3 ui.c $(MAIN_OBJ) $(MINIZ) $(ICON_RES) -o ".\bin\mod\$(OUT_NAME)" -ldwmapi -lgdi32
	xcopy /s /q /i /y "data" ".\bin\mod\data"

zip: release
	cd bin && zip -r $(OUT_NAME).zip $(ZIPPED_FILES)

cli: cli.c fgr
	$(CC) $(WFLAGS) -o $(OUT_NAME) cli.c $(MAIN_OBJ) $(MINIZ)
