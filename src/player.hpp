#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
public:
    Sprite sprite;
    Vector2f position;
    Texture runTexture, idleTexture, attackTexture;

    int frameWidth = 128, frameHeight = 128;
    int currentFrame = 0, attackFrame = 0;
    bool facingRight = true, isRunning = false, isAttacking = false;

    Clock movementClock, attackClock;
    int totalAttackFrames = 4;

    Player(const std::string &spritesPath);
    void update();
    void attack();
    void move();
};

#endif // PLAYER_HPP
