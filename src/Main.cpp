#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>

#include <headers/Game.h>

int main(int argc, char *argv[]) // SDL'nin istediği fonksiyon imzası
{
    Game game;
    game.run();

    return 0;
}
