#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "cloud.hpp"
#include "ground.hpp"
#include "ball.hpp"

class Game
{
public:
    RenderWindow window;
    Texture backgroundTexture, cloud1Texture, cloud2Texture;
    vector<RectangleShape> backgrounds;
    vector<Cloud> clouds;
    Player player;
    Ground ground;
    Ball ball;
    Clock gameClock;
    string assetsPath = "../assets";
    float scrollSpeed = 50.0f;

    Game(const std::string &assetsPath);
    void run();
    void handleEvents();
    void update(float deltaTime);
    void render();
};

#endif // GAME_HPP
