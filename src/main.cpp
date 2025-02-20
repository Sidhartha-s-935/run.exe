#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

void movement(Vector2f &position, Sprite &sprite)
{
    if (Keyboard::isKeyPressed(Keyboard::W))
    {
        position.y -= 0.1;
        sprite.setPosition(position);
    }
    if (Keyboard::isKeyPressed(Keyboard::A))
    {
        position.x -= 0.1;
        sprite.setPosition(position);
    }
    if (Keyboard::isKeyPressed(Keyboard::S))
    {
        position.y += 0.1;
        sprite.setPosition(position);
    }
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        position.x += 0.1;
        sprite.setPosition(position);
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "SFML");
    Texture texture;
    Sprite sprite;
    texture.loadFromFile("/home/notsus/projects/c++project/assets/sprites/player.png");
    sprite.setTexture(texture);
    sprite.setTextureRect(IntRect(0, 0, 55, 37));
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
        Vector2f position = sprite.getPosition();
        movement(position, sprite);
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}