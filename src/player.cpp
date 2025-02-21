#include "player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "ball.hpp"

Player::Player(const std::string &spritesPath)
{
    if (!runTexture.loadFromFile(spritesPath + "/run.png"))
        std::cerr << "Failed to load run texture!" << std::endl;

    if (!idleTexture.loadFromFile(spritesPath + "/idle.png"))
        std::cerr << "Failed to load idle texture!" << std::endl;

    if (!attackTexture.loadFromFile(spritesPath + "/attack.png"))
        std::cerr << "Failed to load attack texture!" << std::endl;

    if (!jumpTexture.loadFromFile(spritesPath + "/jump.png"))
        std::cerr << "Failed to load jump texture!" << std::endl;

    if (!font.loadFromFile("../assets/font.otf"))
    {
        std::cerr << "Error loading font!" << std::endl;
    }

    // Initialize game over text
    Player::gameOverText.setFont(font);
    Player::gameOverText.setString("Game Over!");
    Player::gameOverText.setCharacterSize(80);
    Player::gameOverText.setFillColor(sf::Color::Yellow);
    Player::gameOverText.setStyle(sf::Text::Bold);
    Player::gameOverText.setPosition(500, 300);

    sprite.setTexture(idleTexture);
    position = Vector2f(400, groundLevel);
}

void Player::jump()
{
    if (Keyboard::isKeyPressed(Keyboard::Space) && !isJumping)
    {
        isJumping = true;
        jumpVelocity = jumpStrength;
        jumpFrame = 0; // Start jump animation
        jumpClock.restart();
    }

    if (isJumping)
    {
        float deltaTime = jumpClock.restart().asSeconds();
        jumpVelocity += gravity * deltaTime;
        position.y += jumpVelocity * deltaTime;

        // Determine animation phase
        if (jumpVelocity < -100.0f)
        {
            // Going up animation
            jumpFrame = 4;
        }
        else if (jumpVelocity >= -100.0f && jumpVelocity <= 100.0f)
        {
            // Peak animation (play fully)
            static float peakTime = 0.0f;
            peakTime += deltaTime;

            if (peakTime > 0.05f) // Hold peak frame longer
            {
                jumpFrame = 6; // Peak animation
                peakTime = 0.0f;
            }
        }
        else if (jumpVelocity > 100.0f && position.y < groundLevel)
        {
            // Falling animation (play fully)
            static bool fallAnimPlayed = false;
            if (!fallAnimPlayed)
            {
                jumpFrame = 7;
                fallAnimPlayed = true;
            }
        }

        sprite.setTexture(jumpTexture);
        sprite.setTextureRect(IntRect(jumpFrame * frameWidth, 0, frameWidth, frameHeight));

        // Ground collision check
        if (position.y - 10 >= groundLevel)
        {
            position.y = groundLevel;
            jumpVelocity = 0;
            isJumping = false;

            // Play full falling animation before transitioning
            jumpFrame = 2;
        }
    }
}

void Player::move()
{
    if (isAttacking)
        return;

    float frameSpeed = 0.1f;
    float baseSpeed = 100.0f;
    float airControl = 0.7f; // Reduced control while in air
    bool moving = false;
    float deltaTime = movementClock.getElapsedTime().asSeconds();
    float spriteWidth = frameWidth * sprite.getScale().x;

    // Base scroll movement
    if (!isJumping)
        position.x -= 25 * deltaTime;
    // Calculate actual movement speed
    float moveSpeed = baseSpeed;
    if (isJumping)
        moveSpeed *= airControl;

    if (Keyboard::isKeyPressed(Keyboard::A))
    {
        position.x -= moveSpeed * (isJumping ? airControl : 1.0f) * deltaTime;
        facingRight = false;
        moving = true;
    }
    else if (Keyboard::isKeyPressed(Keyboard::D))
    {
        position.x += moveSpeed * (isJumping ? airControl : 1.0f) * deltaTime;
        facingRight = true;
        moving = true;
    }

    // Screen bounds checking
    const float leftBound = 0.0f;
    const float rightBound = 1280.0f - spriteWidth;

    if (position.x < Ball::radius + 50)
    {
        gameOver = true;
    }
    if (position.x > rightBound)
        position.x = rightBound;

    isRunning = moving;
    sprite.setPosition(position);

    if (!isJumping && movementClock.getElapsedTime().asSeconds() > frameSpeed)
    {
        currentFrame = (currentFrame + 1) % (isRunning ? 8 : 6);
        movementClock.restart();
    }
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

void Player::update()
{
    jump();         // Process jumping first
    if (!isJumping) // Only allow attack if not jumping
    {
        attack();
    }
    move();

    if (!isAttacking && !isJumping)
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