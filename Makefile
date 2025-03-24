all:
	g++ -o Main.exe src/Main.cpp src/Game.cpp src/Player.cpp src/glad.c -Iinclude -Llib -lmingw32 -lSDL2main -lopengl32 -lSDL2
	
