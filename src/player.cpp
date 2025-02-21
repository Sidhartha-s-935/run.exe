#include "player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

Player::Player(const std::string &spritesPath)
{
    if (!runTexture.loadFromFile(spritesPath + "/run.png"))
        std::cerr << "Failed to load run texture!" << std::endl;

    if (!idleTexture.loadFromFile(spritesPath + "/idle.png"))
        std::cerr << "Failed to load idle texture!" << std::endl;

    if (!attackTexture.loadFromFile(spritesPath + "/attack.png"))
        std::cerr << "Failed to load attack texture!" << std::endl;

    sprite.setTexture(idleTexture);
    position = Vector2f(400, 300);
}

void Player::attack()
{
    float frameSpeed = 0.08f;
    float attackMoveSpeed = 0.03f;

    if (Keyboard::isKeyPressed(Keyboard::K) && !isAttacking)
    {
        isAttacking = true;
        attackFrame = 0;
        attackClock.restart();
    }

    if (isAttacking)
    {
        if (attackClock.getElapsedTime().asSeconds() > frameSpeed)
        {
            attackFrame++;
            attackClock.restart();
        }

        if (attackFrame >= totalAttackFrames)
        {
            isAttacking = false;
            attackFrame = 0;
        }

        sprite.setTexture(attackTexture);
        sprite.setTextureRect(IntRect(attackFrame * frameWidth, 0, frameWidth, frameHeight));

        if (attackFrame < totalAttackFrames && attackFrame >= 2)
        {
            position.x += (facingRight ? attackMoveSpeed : -attackMoveSpeed);
        }
    }
}

void Player::move()
{
    if (isAttacking)
        return;

    float frameSpeed = 0.1f;
    float moveSpeed = 100.0f;
    bool moving = false;

    if (Keyboard::isKeyPressed(Keyboard::A))
    {
        position.x -= moveSpeed * movementClock.getElapsedTime().asSeconds();
        facingRight = false;
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        position.x += moveSpeed * movementClock.getElapsedTime().asSeconds();
        facingRight = true;
        moving = true;
    }

    isRunning = moving;
    sprite.setPosition(position);

    if (movementClock.getElapsedTime().asSeconds() > frameSpeed)
    {
        currentFrame = (currentFrame + 1) % (isRunning ? 8 : 6);
        movementClock.restart();
    }
}

void Player::update()
{
    attack();
    move();

    if (!isAttacking)
    {
        sprite.setTexture(isRunning ? runTexture : idleTexture);
        sprite.setTextureRect(IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
    }

    if (!facingRight)
    {
        sprite.setScale(-2.f, 2.f);
        sprite.setOrigin(frameWidth, 0);
    }
    else
    {
        sprite.setScale(2.f, 2.f);
        sprite.setOrigin(0, 0);
    }
}
