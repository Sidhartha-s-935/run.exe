#include "game.hpp"
#include <iostream>

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
}

void Game::run()
{
    while (window.isOpen())
    {
        float deltaTime = gameClock.restart().asSeconds();
        handleEvents();
        update(deltaTime);
        render(); // Move player right
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
    ground.update(scrollSpeed * deltaTime);

    player.update();

    float playerX = player.sprite.getPosition().x;
    float playerWidth = player.sprite.getGlobalBounds().width;
    float windowWidth = window.getSize().x;

    if (playerX < 0)
        player.sprite.setPosition(0, player.sprite.getPosition().y); // Stop at left bound

    if (playerX + playerWidth > windowWidth)
        player.sprite.setPosition(windowWidth - playerWidth, player.sprite.getPosition().y); // Stop at right bound
}

void Game::render()
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
    if (!player.gameOver)
        window.draw(player.sprite);
    else
        window.draw(player.gameOverText);
    window.display();
}