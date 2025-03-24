#include <sdl/SDL.h>

class Player
{
public:
    Player(int x, int y, int width, int height);
    void handleInput(SDL_Event &event); // Oyuncudan gelen girişleri (klavye, fare vs.) işler.
    void update(float deltaTime);       // Oyuncu hareketini ve diğer durumları günceller. mesela left tarafa 5 adim gittiysek ekranda bu pozisyonuna gider
    void render(int screenHeight);      // Oyuncuyu ekranda çizer.

private:
    float x;
    float y;

    float velocityX;
    float velocityY;

    float width;
    float height;

    bool jumping;
    bool walking;

    float walkSpeed;
    float jumpForce;
    float maxJumpHeight;

    float gravity;

    void move(float deltaTime); // Hareket fonksiyonu
};