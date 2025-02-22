#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Ball
{
public:
    constexpr static float radius = 100.f;                  // Radius of the ball (used for positioning)
    sf::Vector2f position = sf::Vector2f(radius + 75, 480); // Position of the ball
    sf::Texture texture;
    sf::Sprite sprite;
    constexpr static int frameCount = 9; // Number of animation frames
    int currentFrame = 0;
    float frameDuration = 0.05f; // Time per frame in seconds
    float elapsedTime = 0.0f;

    Ball();                              // Constructor
    void update(float deltaTime);        // Update animation
    void draw(sf::RenderWindow &window); // Render the sprite
};

#endif // BALL_HPP
