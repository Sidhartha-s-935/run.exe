#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;

class Menu
{
public:
    Menu(const std::string &assetsPath, const Vector2f &windowSize);

    enum class State
    {
        MainMenu,
        Playing,
        GameOver
    };

    void handleInput(const Event &event, RenderWindow &window);
    void update(float deltaTime);
    void draw(RenderWindow &window);
    void showGameOver(int score, int highScore);
    void reset();

    State getCurrentState() const { return currentState; }
    void setState(State newState) { currentState = newState; }

private:
    void initializeButtons();
    void centerText(Text &text, float yPosition);

    Font font;
    State currentState;

    // Main menu elements
    Text titleText;
    Text playButton;
    Text exitButton;

    // Game over elements
    Text gameOverText;
    Text scoreText;
    Text highScoreText;
    Text returnToMenuText;

    float gameOverDelay;
    float currentDelay;

    Vector2f windowSize;
    int selectedButton;
    std::vector<Text *> currentButtons;
};

#endif // MENU_HPP
