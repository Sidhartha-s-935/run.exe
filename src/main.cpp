#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

void movement(Vector2f &position, Sprite &runsprite, int &currentFrame, bool &facingRight, Clock &clock)
{
    float frameSpeed = 0.1f; // Time between frame updates
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

    runsprite.setPosition(position);

    // Update animation frame
    if (moving && clock.getElapsedTime().asSeconds() > frameSpeed)
    {
        currentFrame = (currentFrame + 1) % 8; // Cycle through 8 frames
        clock.restart();
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "SFML");
    Texture runtexture;
    if (!runtexture.loadFromFile("/home/notsus/projects/c++project/assets/sprites/player.png"))
    {
        cerr << "Error loading runsprite sheet!" << endl;
        return -1;
    }

    Sprite runsprite;
    runsprite.setTexture(runtexture);

    Vector2f position(400, 300);
    int frameWidth = 128, frameHeight = 128;
    int currentFrame = 0;
    bool facingRight = true;
    Clock clock;

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

        movement(position, runsprite, currentFrame, facingRight, clock);

        // Set runtexture rect for animation
        runsprite.setTextureRect(IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));

        // Flip runsprite when moving left
        if (!facingRight)
        {
            runsprite.setScale(-1.f, 1.f);
            runsprite.setOrigin(frameWidth, 0);
        }
        else
        {
            runsprite.setScale(1.f, 1.f);
            runsprite.setOrigin(0, 0);
        }

        window.clear();
        window.draw(runsprite);
        window.display();
    }

    return 0;
}
