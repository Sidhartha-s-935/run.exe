#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

void attack(Sprite &sprite, Texture &attackTexture, int &attackFrame, Clock &attackClock,
            bool &isAttacking, bool &isRunning, bool facingRight, Vector2f &position, int totalAttackFrames)
{
    float frameSpeed = 0.08f;
    float attackMoveSpeed = 0.03f; // Distance moved per attack frame

    if (Keyboard::isKeyPressed(Keyboard::K) && !isAttacking)
    {
        isAttacking = true;
        attackFrame = 0; // Start attack animation from frame 0
        attackClock.restart();
    }

    if (isAttacking)
    {
        if (attackClock.getElapsedTime().asSeconds() > frameSpeed)
        {
            attackFrame++; // Move to next attack frame
            attackClock.restart();
        }

        if (attackFrame >= totalAttackFrames) // If attack animation is finished
        {
            isAttacking = false; // Resume movement
            attackFrame = 0;     // Reset attack frame
        }

        sprite.setTexture(attackTexture);
        sprite.setTextureRect(IntRect(attackFrame * 128, 0, 128, 128));

        // Move forward slightly while attacking
        if (attackFrame < totalAttackFrames && attackFrame >= 2)
        {
            position.x += (facingRight ? attackMoveSpeed : -attackMoveSpeed);
        }
    }
}

void movement(Vector2f &position, Sprite &sprite, Texture &runTexture, Texture &idleTexture,
              int &currentFrame, bool &facingRight, Clock &clock, bool &isRunning, bool isAttacking)
{
    if (isAttacking)
        return; // Stop movement when attacking

    float frameSpeed = 0.1f;
    bool moving = false;

    if (Keyboard::isKeyPressed(Keyboard::W))
    {
        position.y -= 0.1;
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::A))
    {
        position.x -= 0.1;
        facingRight = false;
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::S))
    {
        position.y += 0.1;
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        position.x += 0.1;
        facingRight = true;
        moving = true;
    }

    sprite.setPosition(position);
    isRunning = moving;

    if (clock.getElapsedTime().asSeconds() > frameSpeed)
    {
        currentFrame = (currentFrame + 1) % (isRunning ? 8 : 6); // Run: 8 frames, Idle: 6 frames
        clock.restart();
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "SFML");

    // Load textures
    Texture runTexture, idleTexture, attackTexture;
    if (!runTexture.loadFromFile("/home/notsus/projects/c++project/assets/sprites/run.png") ||
        !idleTexture.loadFromFile("/home/notsus/projects/c++project/assets/sprites/idle.png") ||
        !attackTexture.loadFromFile("/home/notsus/projects/c++project/assets/sprites/attack.png"))
    {
        cerr << "Error loading sprite sheets!" << endl;
        return -1;
    }

    Sprite sprite;
    sprite.setTexture(idleTexture);

    Vector2f position(400, 300);
    int frameWidth = 128, frameHeight = 128;
    int currentFrame = 0, attackFrame = 0;
    bool facingRight = true, isRunning = false, isAttacking = false;
    Clock movementClock, attackClock;
    int totalAttackFrames = 4; // Adjust according to attack animation frames

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        attack(sprite, attackTexture, attackFrame, attackClock, isAttacking, isRunning, facingRight, position, totalAttackFrames);
        movement(position, sprite, runTexture, idleTexture, currentFrame, facingRight, movementClock, isRunning, isAttacking);

        // Set texture based on state
        if (!isAttacking)
        {
            sprite.setTexture(isRunning ? runTexture : idleTexture);
            sprite.setTextureRect(IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
        }

        // Flip sprite if facing left
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

        sprite.setPosition(position); // Update position after attack movement

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
