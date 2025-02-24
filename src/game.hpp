#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "player.hpp"
#include "cloud.hpp"
#include "ground.hpp"
#include "ball.hpp"
#include "menu.hpp"

class Game
{
public:
    RenderWindow window;
    Texture backgroundTexture, cloud1Texture, cloud2Texture, cloud3Texture, cloud4Texture, backgroundNightTexture;
    vector<RectangleShape> backgrounds;
    vector<Cloud> clouds;
    Player player;
    Ground ground;
    Ball ball;
    Menu menu;
    Clock gameClock;
    Font font;
    Text scoreText;
    Text highscoreText;
    Music music;
    string highscorePath = "../assets/highscore.txt";
    int scoreval = 0;
    int highscore = 0;
    string assetsPath = "../assets";
    float scrollSpeed = 50.0f;
    float backgroundTransitionAlpha;
    bool isNightMode;
    float transitionProgress;
    float targetTransitionProgress;
    const float TRANSITION_SPEED = 0.5f;

    Game(const std::string &assetsPath);
    void run();
    void handleEvents();
    void score(float deltaTime);
    void update(float deltaTime);
    void render(float deltaTime);
    void updateBackgroundTransition(float deltaTime);
};

#endif // GAME_HPP
