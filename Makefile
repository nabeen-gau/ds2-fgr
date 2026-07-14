CC := gcc
NCC := ./tools/llvm-mingw-20260616-msvcrt-x86_64/llvm-mingw-20260616-msvcrt-x86_64/bin/gcc.exe

WFLAGS := -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CFLAGS := -ggdb $(WFLAGS)
CPPFLAGS := -Iother
MINIZ := other/miniz.c
SOURCES := main.c $(MINIZ)
MAIN_OBJ := fgr.o
OUT_NAME := DS2FGR
OUT_EXE := $(OUT_NAME).exe
ICON_RES := resource.res

ZIPPED_FILES := mod/$(OUT_EXE) mod/data/enc_regulation.bnd.dcx mod/data/map/ mod/data/menu/ mod/data/Param/ mod/data/ezstate/ dinput8.dll modengine.ini
MINIZ_OBJ := miniz.o

.PHONY: all ui cli release zip test dump-esd fgr-release clean

all: ui

item_info.c: data/item-lot.txt reformat-item-info.py
	python reformat-item-info.py

$(ICON_RES): resource.rc
	windres resource.rc -O coff -o $(ICON_RES)

$(MINIZ_OBJ): $(MINIZ)
	$(CC) $(WFLAGS) -c -o miniz.o $(MINIZ)

test: test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o test test.c $(MINIZ)

dump-esd: dump-esd.c
	$(CC) -O3 $(CFLAGS) -o dump-esd dump-esd.c

$(MAIN_OBJ): main.c $(ICON_RES) item_info.c
	$(CC) -c main.c $(WFLAGS) $(CPPFLAGS) -o $(MAIN_OBJ)

fgr-release: $(MAIN_OBJ)

fgr-release: main.c $(ICON_RES) item_info.c
	$(CC) -O3 -DRELEASE -c main.c $(WFLAGS) $(CPPFLAGS) -o $(MAIN_OBJ)

ui: ui.c $(MAIN_OBJ) $(MINIZ_OBJ)
	$(CC) ui.c $(MAIN_OBJ) $(MINIZ_OBJ) $(ICON_RES) -o $(OUT_NAME) -ldwmapi -lgdi32

release: ui.c fgr-release
	if not exist ".\bin" mkdir ".\bin"
	if exist ".\bin\mod" rmdir /s /q ".\bin\mod"
	mkdir ".\bin\mod"
	$(CC) -O3 ui.c $(MAIN_OBJ) $(MINIZ_OBJ) $(ICON_RES) -o ".\bin\mod\$(OUT_NAME)" -ldwmapi -lgdi32
	xcopy /s /q /i /y "data" ".\bin\mod\data"

zip: release
	if exist ".\bin\$(OUT_NAME).zip" del ".\bin\$(OUT_NAME).zip"
	cd bin && zip -r $(OUT_NAME).zip $(ZIPPED_FILES)

cli: cli.c $(MAIN_OBJ)
	$(CC) $(WFLAGS) -o $(OUT_NAME) cli.c $(MAIN_OBJ) $(MINIZ)

pdb: cli.c $(MAIN_OBJ)
	$(NCC) $(CFLAGS) $(CPPFLAGS) -Wl,--pdb=$(OUT_NAME).pdb -o $(OUT_NAME) cli.c $(MAIN_OBJ) $(MINIZ_OBJ)

