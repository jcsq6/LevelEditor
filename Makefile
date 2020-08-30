EXE_FILE = bin/LevelEditor
OBJ_FILES = obj/main.o obj/tinyFileDialog.o
FLAGS = 
#-ggdb

exe : $(EXE_FILE)

debug: FLAGS += -ggdb
debug : exe

windows: EXE_FILE = bin/LevelEditor.exe
windows: exe

$(EXE_FILE) : $(OBJ_FILES)
	g++ -o $(EXE_FILE) $(OBJ_FILES) `pkg-config --libs sdl2 SDL2_image SDL2_ttf` $(FLAGS)

obj/main.o : src/main.cpp
	g++ -c -o obj/main.o src/main.cpp $(FLAGS)

obj/tinyFileDialog.o : src/tinyfiledialogs.c src/tinyfiledialogs.h
	g++ -c -o obj/tinyFileDialog.o src/tinyfiledialogs.c $(FLAGS)

run : exe
	$(EXE_FILE) &

clean:
	rm -f *.o obj/main.o obj/tinyFileDialog.o
