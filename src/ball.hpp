#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Ball
{
public:
    sf::CircleShape shape;
    constexpr static float radius = 100.f; // Radius of the ball
    sf::Vector2f position;                 // Position of the ball
    sf::Color color = sf::Color::Red;
    Ball();                              // Constructor
    void update();                       // Update ball state if needed
    void draw(sf::RenderWindow &window); // Render the ball
};

#endif // BALL_HPP
