#include "ball.hpp"

Ball::Ball()
{
    if (!texture.loadFromFile("../assets/ball/Run.png"))
    {
        throw std::runtime_error("Failed to load ball sprite.");
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 200, 200)); // Assuming each frame is 200x200 pixels
    sprite.setOrigin(radius, radius);
    sprite.setPosition(position);
}

void Ball::update(float deltaTime)
{
    elapsedTime += deltaTime;
    if (elapsedTime >= frameDuration)
    {
        elapsedTime = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
        sprite.setTextureRect(sf::IntRect(currentFrame * 128, 0, 128, 128));
        sprite.setScale(3.f, 3.f); // Move to next frame
    }
}

void Ball::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}
