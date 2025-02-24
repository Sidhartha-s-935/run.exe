#include "menu.hpp"

Menu::Menu(const std::string &assetsPath, const Vector2f &windowSize)
    : windowSize(windowSize), currentState(State::MainMenu), selectedButton(0), gameOverDelay(3.0f) // Show game over screen for 3 seconds before showing menu options
      ,
      currentDelay(0.0f)
{
    if (!font.loadFromFile(assetsPath + "/font.otf"))
    {
        throw std::runtime_error("Failed to load font!");
    }

    // Initialize title
    titleText.setFont(font);
    titleText.setString("Run.exe");
    titleText.setCharacterSize(72);
    titleText.setFillColor(Color::White);
    centerText(titleText, windowSize.y * 0.25f);

    // Initialize buttons
    playButton.setFont(font);
    playButton.setString("Play");
    playButton.setCharacterSize(48);
    playButton.setFillColor(Color::White);
    centerText(playButton, windowSize.y * 0.5f);

    exitButton.setFont(font);
    exitButton.setString("Exit");
    exitButton.setCharacterSize(48);
    exitButton.setFillColor(Color::White);
    centerText(exitButton, windowSize.y * 0.6f);

    // Initialize game over elements
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(72);
    gameOverText.setFillColor(Color::White);
    centerText(gameOverText, windowSize.y * 0.3f);

    scoreText.setFont(font);
    scoreText.setCharacterSize(48);
    scoreText.setFillColor(Color::White);
    centerText(scoreText, windowSize.y * 0.45f);

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(48);
    highScoreText.setFillColor(Color::White);
    centerText(highScoreText, windowSize.y * 0.55f);

    returnToMenuText.setFont(font);
    returnToMenuText.setString("Press ENTER to return to menu");
    returnToMenuText.setCharacterSize(36);
    returnToMenuText.setFillColor(Color::White);
    centerText(returnToMenuText, windowSize.y * 0.7f);

    initializeButtons();
}

void Menu::initializeButtons()
{
    currentButtons.clear();

    if (currentState == State::MainMenu)
    {
        currentButtons = {&playButton, &exitButton};
    }

    // Update the selected button color
    for (size_t i = 0; i < currentButtons.size(); ++i)
    {
        currentButtons[i]->setFillColor(i == selectedButton ? Color::Yellow : Color::White);
    }
}

void Menu::centerText(Text &text, float yPosition)
{
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(Vector2f(windowSize.x / 2.0f, yPosition));
}

void Menu::handleInput(const Event &event, RenderWindow &window)
{
    if (currentState == State::MainMenu)
    {
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Up)
            {
                selectedButton = (selectedButton - 1 + currentButtons.size()) % currentButtons.size();
                initializeButtons();
            }
            else if (event.key.code == Keyboard::Down)
            {
                selectedButton = (selectedButton + 1) % currentButtons.size();
                initializeButtons();
            }
            else if (event.key.code == Keyboard::Return)
            {
                if (selectedButton == 0)
                { // Play button
                    currentState = State::Playing;
                }
                else if (selectedButton == 1)
                { // Exit button
                    window.close();
                }
            }
        }
    }
    else if (currentState == State::GameOver && currentDelay >= gameOverDelay)
    {
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Return)
        {
            currentState = State::MainMenu;
            initializeButtons();
        }
    }
}

void Menu::update(float deltaTime)
{
    if (currentState == State::GameOver)
    {
        currentDelay += deltaTime;
    }
}

void Menu::draw(RenderWindow &window)
{
    if (currentState == State::MainMenu)
    {
        window.draw(titleText);
        for (const auto &button : currentButtons)
        {
            window.draw(*button);
        }
    }
    else if (currentState == State::GameOver)
    {
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(highScoreText);
        if (currentDelay >= gameOverDelay)
        {
            window.draw(returnToMenuText);
        }
    }
}

void Menu::showGameOver(int score, int highScore)
{
    currentState = State::GameOver;
    currentDelay = 0.0f;

    scoreText.setString("Score: " + std::to_string(score));
    centerText(scoreText, windowSize.y * 0.45f);

    highScoreText.setString("High Score: " + std::to_string(highScore));
    centerText(highScoreText, windowSize.y * 0.55f);
}

void Menu::reset()
{
    currentState = State::MainMenu;
    selectedButton = 0;
    currentDelay = 0.0f;
    initializeButtons();
}