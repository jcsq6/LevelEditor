EXE_FILE = bin/LevelEditor
OBJ_FILES = obj/main.o obj/tinyFileDialog.o
FLAGS = -w -fpermissive --std=c++17
CC = g++

exe : clean
exe : $(EXE_FILE)

debug: FLAGS += -ggdb
debug : exe

windows: EXE_FILE = bin/LevelEditor.exe
windows: CC = x86_64-w64-mingw32-g++
windows: FLAGS += -lole32 -lcomdlg32 -lmingw32 -lSDL2main -lSDL2 -mwindows -Wl,--no-undefined -Wl,--dynamicbase -Wl,--nxcompat -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -static-libgcc
windows: exe

$(EXE_FILE) : $(OBJ_FILES)
	$(CC) -o $(EXE_FILE) $(OBJ_FILES) `pkg-config --libs sdl2 SDL2_image SDL2_ttf` $(FLAGS)

obj/main.o : src/main.cpp
	$(CC) -c -o obj/main.o src/main.cpp $(FLAGS)

obj/tinyFileDialog.o : src/tinyfiledialogs.c src/tinyfiledialogs.h
	$(CC) -c -o obj/tinyFileDialog.o src/tinyfiledialogs.c $(FLAGS)

run : exe
	$(EXE_FILE) &

clean:
	rm -f *.o obj/main.o obj/tinyFileDialog.o
