all:
	g++ -o Main.exe src/Main.cpp src/Game.cpp src/Player.cpp src/Camera.cpp src/Chunk.cpp src/World.cpp src/glad.c src/TextRenderer.cpp src/physics/Collision.cpp src/physics/AABB.cpp src/debugRenderer.cpp -Iinclude -Llib -lmingw32 -lSDL2main -lopengl32 -lSDL2 -lfreetype -mavx
	
