#include "ball.hpp"

Ball::Ball()
{
    shape.setRadius(radius);
    shape.setFillColor(color);
    shape.setOrigin(radius, radius);
    position = sf::Vector2f(radius, 500);
    shape.setPosition(position);
}

void Ball::update()
{
    
}

void Ball::draw(sf::RenderWindow &window)
{
    window.draw(shape);
}
