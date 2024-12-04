includes = includes
lib = lib
out = game/game_of_life.exe
c_files = src/main.c src/gol.c

all : $(OBJS)
#-Wl,-subsystem,windows
	gcc $(c_files) -I$(includes) -L$(lib) -lmingw32 -lSDL2main -lSDL2 -o $(out)