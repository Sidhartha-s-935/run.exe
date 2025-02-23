#include "game.hpp"
#include <iostream>
#include <fstream>

Game::Game(const std::string &assetsPath)
    : window(VideoMode(1280, 720), "SFML"),
      player(assetsPath + "/sprites"),
      ground(assetsPath + "/world/ground"),
      scrollSpeed(200.0f) // Adjust speed as necessary
{
    window.setVerticalSyncEnabled(true);
    srand(time(0));

    std::string worldPath = assetsPath + "/world";

    if (!backgroundTexture.loadFromFile(worldPath + "/background.png"))
        std::cerr << "Failed to load background texture!" << std::endl;

    if (!cloud1Texture.loadFromFile(worldPath + "/cloud1.png"))
        std::cerr << "Failed to load cloud1 texture!" << std::endl;

    if (!cloud2Texture.loadFromFile(worldPath + "/cloud2.png"))
        std::cerr << "Failed to load cloud2 texture!" << std::endl;

    for (int i = 0; i < 3; i++)
    {
        RectangleShape background(Vector2f(1280, 720));
        background.setTexture(&backgroundTexture);
        background.setPosition(i * 1280 - 1, 0);
        backgrounds.push_back(background);
    }

    initializeClouds(clouds, cloud1Texture, cloud2Texture, scrollSpeed, 1280.0f * 3);

    if (!music.openFromFile(assetsPath + "/music.mp3"))
    {
        std::cerr << "Failed to load background music!" << std::endl;
    }
    else
    {
        music.setLoop(true);
        music.play();
    }
}

void Game::run()
{
    while (window.isOpen())
    {
        float deltaTime = gameClock.restart().asSeconds();
        handleEvents();
        update(deltaTime);
        render(deltaTime); // Move player right
    }
}

void Game::handleEvents()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
    }
}

void Game::update(float deltaTime)
{
    for (auto &background : backgrounds)
    {
        background.move(scrollSpeed * deltaTime, 0);
        if (background.getPosition().x >= 1280)
        {
            float leftmostX = 1280.0f;
            for (const auto &bg : backgrounds)
            {
                leftmostX = std::min(leftmostX, bg.getPosition().x);
            }
            background.setPosition(leftmostX - 1280 + 1, 0);
        }
    }

    updateClouds(clouds, deltaTime, cloud1Texture, cloud2Texture, -scrollSpeed);
    ground.update(deltaTime, scrollSpeed * deltaTime, player);
    score(deltaTime);

    if (scoreval % 500 == 0)
    {
        if (player.frameSpeed > 0.05f && scoreval % 1000 == 0)
            player.frameSpeed -= 0.01f;
        if (ball.frameDuration > 0.04f && scoreval % 2500 == 0)
            ball.frameDuration -= 0.01f;
        scrollSpeed += 50.0f;
        player.pushBackAmount += 1.f;
    }

    if (player.gameOver)
    {
    }

    player.update();

    float playerX = player.sprite.getPosition().x;
    float playerWidth = player.sprite.getGlobalBounds().width;
    float windowWidth = window.getSize().x;

    if (playerX < 0)
        player.sprite.setPosition(0, player.sprite.getPosition().y); // Stop at left bound

    if (playerX + playerWidth > windowWidth)
        player.sprite.setPosition(windowWidth - playerWidth, player.sprite.getPosition().y); // Stop at right bound
}

void Game::score(float deltaTime)
{
    ifstream file(highscorePath);
    if (file.is_open())
    {
        file >> highscore;
        file.close();
    }
    font.loadFromFile("../assets/font.otf");
    if (!player.gameOver)
    {
        scoreval += 1;
        scoreText.setString("Score: " + std::to_string(scoreval));
        highscoreText.setString("Highscore: " + std::to_string(highscore));
        highscoreText.setPosition(1100, 75);
        highscoreText.setFont(font);
        highscoreText.setCharacterSize(32);
        highscoreText.setFillColor(Color::White);
        scoreText.setPosition(1100, 50);
        scoreText.setFont(font);
        scoreText.setCharacterSize(32);
        scoreText.setFillColor(Color::White);
    }
    else
    {
        if (scoreval > highscore)
        {
            highscore = scoreval;
        }
        ofstream file(highscorePath);
        if (file.is_open())
        {
            file << highscore;
            file.close();
        }
    }
}

void Game::render(float deltaTime)
{
    window.clear();

    for (const auto &background : backgrounds)
    {
        window.draw(background);
    }

    for (const auto &cloud : clouds)
    {
        window.draw(cloud.sprite);
    }

    ground.draw(window);
    ball.update(deltaTime);
    ball.draw(window);
    if (!player.gameOver)
        window.draw(player.sprite);
    else
        window.draw(player.gameOverText);
    window.draw(scoreText);
    window.draw(highscoreText);
    window.display();
}