CC := gcc
NCC := ./tools/llvm-mingw-20260616-msvcrt-x86_64/llvm-mingw-20260616-msvcrt-x86_64/bin/gcc.exe

WFLAGS := -Wall -Wextra -Wno-unused-function -Wno-initializer-overrides
CFLAGS := -ggdb $(WFLAGS)
CPPFLAGS := -Iother
MINIZ := other/miniz.c
DBG_OBJ := dbg.o
MAIN_OBJ := fgr.o
OUT_NAME := DS2FGR
OUT_EXE := $(OUT_NAME).exe
ICON_RES := resource.res
MAIN_DEPS := main.c item_info.c esd.c foggates.c common.c other/miniz.h nob.h ht.h $(ICON_RES)

ZIPPED_FILES := mod/$(OUT_EXE) mod/data/enc_regulation.bnd.dcx mod/data/map/ mod/data/menu/ mod/data/Param/ mod/data/ezstate/ dinput8.dll modengine.ini
MINIZ_OBJ := miniz.o

all: cli

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

# Debug
$(DBG_OBJ): $(MAIN_DEPS)
	$(CC) -c main.c $(CFLAGS) $(CPPFLAGS) -o $(DBG_OBJ)

# Release
$(MAIN_OBJ): $(MAIN_DEPS)
	$(CC) -O3 -DRELEASE -c main.c $(WFLAGS) $(CPPFLAGS) -o $(MAIN_OBJ)

ui: ui.c $(DBG_OBJ) $(MINIZ_OBJ)
	$(CC) ui.c $(DBG_OBJ) $(MINIZ_OBJ) $(ICON_RES) -o $(OUT_NAME) -ldwmapi -lgdi32 -luxtheme

release: ui.c $(MAIN_OBJ) $(MINIZ_OBJ) $(ICON_RES)
	if not exist ".\bin" mkdir ".\bin"
	if exist ".\bin\mod" rmdir /s /q ".\bin\mod"
	mkdir ".\bin\mod"
	$(CC) -O3 ui.c $(MAIN_OBJ) $(MINIZ_OBJ) $(ICON_RES) -o ".\bin\mod\$(OUT_NAME)" -ldwmapi -lgdi32 -luxtheme
	xcopy /s /q /i /y "data" ".\bin\mod\data"

zip: release
	if exist ".\bin\$(OUT_NAME).zip" del ".\bin\$(OUT_NAME).zip"
	cd bin && zip -r $(OUT_NAME).zip $(ZIPPED_FILES)

cli: cli.c $(DBG_OBJ) $(MINIZ_OBJ)
	$(CC) $(CFLAGS) -o $(OUT_NAME) cli.c $(DBG_OBJ) $(MINIZ_OBJ)

pdb: cli.c $(DBG_OBJ) $(MINIZ_OBJ)
	$(NCC) $(CFLAGS) $(CPPFLAGS) -Wl,--pdb=$(OUT_NAME).pdb -o $(OUT_NAME) cli.c $(DBG_OBJ) $(MINIZ_OBJ)

clean:
	del *.exe
	del *.o
	del *.pdb
	if exist settings.ini del settings.ini
	if exist $(ICON_RES) del $(ICON_RES)
	if exist output\ezstate rmdir /s /q output\ezstate
	if exist output\log rmdir /s /q output\log
	if exist output\map rmdir /s /q output\map
	if exist output\menu rmdir /s /q output\menu
	if exist output\Param rmdir /s /q output\Param
	if exist output\enc_regulation.bnd.dcx del output\enc_regulation.bnd.dcx
	if exist bin\mod rmdir /s /q bin\mod
	if exist bin\DS2FGR.zip del bin\DS2FGR.zip
