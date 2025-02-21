#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
public:
    Sprite sprite;
    Vector2f position;
    Texture runTexture, idleTexture, attackTexture, jumpTexture;
    Font font;
    Text gameOverText;

    int frameWidth = 128, frameHeight = 128;
    int currentFrame = 0, attackFrame = 0, jumpFrame = 0;
    bool facingRight = true, isRunning = false, isAttacking = false, isJumping = false, gameOver = false;
    ;

    Clock movementClock, attackClock, jumpClock;
    int totalAttackFrames = 4;
    int totalJumpFrames = 9;
    float jumpVelocity = 0.0f;
    float jumpStrength = -800.0f;
    float gravity = 3000.0f;
    float groundLevel = 305.0f;

    Player(const std::string &spritesPath);
    void update();
    void attack();
    void move();
    void jump();
};

#endif // PLAYER_HPP
