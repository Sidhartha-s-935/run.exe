#include "ball.hpp"

Ball::Ball()
{
    shape.setRadius(radius);
    shape.setFillColor(color);
    shape.setOrigin(radius, radius);      // Set origin to center
    position = sf::Vector2f(radius, 500); // Left end of the screen at y = 500
    shape.setPosition(position);
}

void Ball::update()
{
    // Add movement logic if needed (currently static)
}

void Ball::draw(sf::RenderWindow &window)
{
    window.draw(shape);
}
