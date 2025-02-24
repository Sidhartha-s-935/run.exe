#include "game.hpp"
#include <iostream>
#include <fstream>

Game::Game(const std::string &assetsPath)
    : window(VideoMode(1280, 720), "SFML"),
      menu(assetsPath, Vector2f(1280, 720)),
      player(assetsPath + "/sprites"),
      ground(assetsPath + "/world/ground"),
      scrollSpeed(200.0f), // Adjust speed as necessary
      backgroundTransitionAlpha(0.0f),
      isNightMode(false),
      transitionProgress(0.0f),
      targetTransitionProgress(0.0f)
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

    if (!cloud3Texture.loadFromFile(worldPath + "/cloud3.png"))
        std::cerr << "Failed to load cloud3 texture!" << std::endl;

    if (!cloud4Texture.loadFromFile(worldPath + "/cloud4.png"))
        std::cerr << "Failed to load cloud4 texture!" << std::endl;

    if (!backgroundNightTexture.loadFromFile(worldPath + "/background-night.png"))
        std::cerr << "Failed to load night background texture!" << std::endl;

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

void Game::updateBackgroundTransition(float deltaTime)
{
    // Calculate target state based on score
    float newTarget = (scoreval / 2000) % 2 == 1 ? 1.0f : 0.0f;

    // Update target if it changed
    if (targetTransitionProgress != newTarget)
    {
        targetTransitionProgress = newTarget;
    }

    // Smoothly interpolate current transition progress toward target
    if (transitionProgress < targetTransitionProgress)
    {
        transitionProgress = std::min(transitionProgress + TRANSITION_SPEED * deltaTime, targetTransitionProgress);
    }
    else if (transitionProgress > targetTransitionProgress)
    {
        transitionProgress = std::max(transitionProgress - TRANSITION_SPEED * deltaTime, targetTransitionProgress);
    }

    // Update background colors based on transition progress
    for (auto &background : backgrounds)
    {
        background.setTexture(&backgroundTexture); // Always set to day texture
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
        menu.handleInput(event, window);
    }
}

void Game::update(float deltaTime)
{
    menu.update(deltaTime);

    if (menu.getCurrentState() == Menu::State::Playing)
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
        updateBackgroundTransition(deltaTime);
        updateClouds(clouds, deltaTime,
                     cloud1Texture, cloud2Texture,
                     cloud3Texture, cloud4Texture,
                     -scrollSpeed, scoreval);
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
            player.sprite.setPosition(windowWidth - playerWidth, player.sprite.getPosition().y);

        if (player.gameOver)
        {
            menu.showGameOver(scoreval, highscore);
        } // Stop at right bound
    }
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
    if (menu.getCurrentState() == Menu::State::Playing)
    {
        for (const auto &background : backgrounds)
        {
            window.draw(background);
        }

        // Draw night backgrounds with transparency based on transition
        if (transitionProgress > 0.0f)
        {
            for (const auto &background : backgrounds)
            {
                RectangleShape nightBg = background;
                nightBg.setTexture(&backgroundNightTexture);
                nightBg.setFillColor(Color(255, 255, 255,
                                           static_cast<uint8_t>(255 * transitionProgress)));
                window.draw(nightBg);
            }
        }

        // Draw clouds
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
    }
    menu.draw(window);
    window.display();
}