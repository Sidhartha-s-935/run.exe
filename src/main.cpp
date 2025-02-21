#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <filesystem>
#include <ctime>

using namespace std;
using namespace sf;

// Ground tile structure to store tile information

struct Cloud
{
    Sprite sprite;
    float speed;

    // Constructor for initial spread-out cloud generation
    Cloud(const Texture &texture, float baseSpeed, float initialX)
    {
        sprite.setTexture(texture);
        // Slightly reduced scale variation for more consistency
        float scale = 0.8f + (rand() % 30) / 100.0f;
        sprite.setScale(scale, scale);

        // More controlled height variation
        float maxHeight = 720 * 0.75f;
        float minHeight = 50;
        float height = minHeight + (rand() % int(maxHeight - minHeight));

        sprite.setPosition(initialX, height);
        speed = baseSpeed * (0.9f + (rand() % 20) / 100.0f);
    }

    // Constructor for new clouds during gameplay
    Cloud(const Texture &texture, float baseSpeed) : Cloud(texture, baseSpeed, 1280.0f) {}

    void reset(float screenWidth)
    {
        float maxHeight = 720 * 0.75f;
        float minHeight = 50;
        sprite.setPosition(
            screenWidth + sprite.getGlobalBounds().width,
            minHeight + (rand() % int(maxHeight - minHeight)));
    }
};

void attack(Sprite &sprite, Texture &attackTexture, int &attackFrame, Clock &attackClock,
            bool &isAttacking, bool &isRunning, bool facingRight, Vector2f &position, int totalAttackFrames)
{
    float frameSpeed = 0.08f;
    float attackMoveSpeed = 0.03f;

    if (Keyboard::isKeyPressed(Keyboard::K) && !isAttacking)
    {
        isAttacking = true;
        attackFrame = 0;
        attackClock.restart();
    }

    if (isAttacking)
    {
        if (attackClock.getElapsedTime().asSeconds() > frameSpeed)
        {
            attackFrame++;
            attackClock.restart();
        }

        if (attackFrame >= totalAttackFrames)
        {
            isAttacking = false;
            attackFrame = 0;
        }

        sprite.setTexture(attackTexture);
        sprite.setTextureRect(IntRect(attackFrame * 128, 0, 128, 128));

        if (attackFrame < totalAttackFrames && attackFrame >= 2)
        {
            position.x += (facingRight ? attackMoveSpeed : -attackMoveSpeed);
        }
    }
}

void movement(Vector2f &position, Sprite &sprite, Texture &runTexture, Texture &idleTexture,
              int &currentFrame, bool &facingRight, Clock &clock, bool &isRunning, bool isAttacking)
{
    if (isAttacking)
        return;

    float frameSpeed = 0.1f;
    float moveSpeed = 100.0f; // Increased movement speed
    bool moving = false;

    if (Keyboard::isKeyPressed(Keyboard::W))
    {
        position.y -= moveSpeed * clock.getElapsedTime().asSeconds();
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::A))
    {
        position.x -= moveSpeed * clock.getElapsedTime().asSeconds();
        facingRight = false;
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::S))
    {
        position.y += moveSpeed * clock.getElapsedTime().asSeconds();
        moving = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        position.x += moveSpeed * clock.getElapsedTime().asSeconds();
        facingRight = true;
        moving = true;
    }

    sprite.setPosition(position);
    isRunning = moving;

    if (clock.getElapsedTime().asSeconds() > frameSpeed)
    {
        currentFrame = (currentFrame + 1) % (isRunning ? 8 : 6);
        clock.restart();
    }
}

void initializeClouds(vector<Cloud> &clouds, const Texture &cloud1Texture,
                      const Texture &cloud2Texture, float scrollSpeed, float totalWidth)
{
    // Calculate number of clouds based on screen width
    int numClouds = 12; // Desired number of initial clouds
    float cloudSpacing = totalWidth / numClouds;

    for (int i = 0; i < numClouds; i++)
    {
        // Create clouds at regular intervals across the total width
        float xPos = i * cloudSpacing + (rand() % int(cloudSpacing * 0.5f)); // Add some randomness to x position
        clouds.push_back(Cloud(
            rand() % 2 == 0 ? cloud1Texture : cloud2Texture,
            scrollSpeed,
            xPos));
    }
}

void updateClouds(vector<Cloud> &clouds, float deltaTime, const Texture &cloud1Texture,
                  const Texture &cloud2Texture, float scrollSpeed)
{
    for (auto &cloud : clouds)
    {
        cloud.sprite.move(-cloud.speed * deltaTime, 0);

        if (cloud.sprite.getPosition().x < -cloud.sprite.getGlobalBounds().width)
        {
            cloud.reset(1280.0f);
        }
    }

    // Maintain cloud count
    const int desiredCloudCount = 12;
    if (clouds.size() < desiredCloudCount)
    {
        clouds.push_back(Cloud(rand() % 2 == 0 ? cloud1Texture : cloud2Texture, scrollSpeed));
    }
}

struct GroundTile
{
    Sprite sprite;
    int tileNumber;
    bool isTop;
};

class Ground
{
private:
    vector<vector<GroundTile>> tiles;
    vector<unique_ptr<Texture>> tileTextures; // Store textures using unique_ptr
    const vector<int> topTiles = {1, 2, 3, 5, 6, 7, 8, 18, 33, 34, 49, 50, 51, 54, 55};
    const vector<int> diagonalTiles = {41, 42};
    const vector<int> elevatedTiles = {31, 32};
    const vector<int> fillerTiles = {4, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20};
    const int tileSize = 32;
    const int gridHeight = 8;
    int currentLeftmostTileIndex = 0;

public:
    Ground(const string &basePath)
    {
        // Load all tile textures
        for (int i = 1; i <= 96; i++)
        {
            auto texture = make_unique<Texture>();
            string filename = basePath + "/Tile_" + (i < 10 ? "0" : "") + to_string(i) + ".png";

            if (!texture->loadFromFile(filename))
            {
                cerr << "Failed to load texture: " << filename << endl;
                // Add a blank texture to maintain index alignment
                tileTextures.push_back(make_unique<Texture>());
                continue;
            }
            tileTextures.push_back(move(texture));
        }

        generateInitialGround();
    }

    void generateInitialGround()
    {
        int initialColumns = (1280 / tileSize) + 2;

        for (int x = 0; x < initialColumns; x++)
        {
            vector<GroundTile> column;

            // Generate top tile
            GroundTile topTile;
            int randomTopIndex = rand() % topTiles.size();
            int tileNumber = topTiles[randomTopIndex];
            if (tileNumber <= tileTextures.size() && tileTextures[tileNumber - 1])
            {
                topTile.tileNumber = tileNumber;
                topTile.sprite.setTexture(*tileTextures[tileNumber - 1]);
                topTile.sprite.setPosition(x * tileSize, 720 - (gridHeight * tileSize));
                topTile.isTop = true;
                column.push_back(topTile);
            }

            // Fill below with random filler tiles
            for (int y = 1; y < gridHeight; y++)
            {
                GroundTile fillerTile;
                int randomFillerIndex = rand() % fillerTiles.size();
                int fillerTileNumber = fillerTiles[randomFillerIndex];
                if (fillerTileNumber <= tileTextures.size() && tileTextures[fillerTileNumber - 1])
                {
                    fillerTile.tileNumber = fillerTileNumber;
                    fillerTile.sprite.setTexture(*tileTextures[fillerTileNumber - 1]);
                    fillerTile.sprite.setPosition(x * tileSize, 720 - ((gridHeight - y) * tileSize));
                    fillerTile.isTop = false;
                    column.push_back(fillerTile);
                }
            }

            tiles.push_back(column);
        }
    }

    void update(float scrollAmount)
    {
        // Move all tiles
        for (auto &column : tiles)
        {
            for (auto &tile : column)
            {
                tile.sprite.move(-scrollAmount, 0);
            }
        }

        // Check if we need to remove leftmost column and add a new one on the right
        if (!tiles.empty() && !tiles[0].empty() &&
            tiles[0][0].sprite.getPosition().x < -tileSize)
        {

            tiles.erase(tiles.begin());

            // Add new column on the right
            vector<GroundTile> newColumn;
            float rightmostX = tiles.back()[0].sprite.getPosition().x + tileSize;

            // Generate top tile
            GroundTile topTile;
            int randomTopIndex = rand() % topTiles.size();
            int tileNumber = topTiles[randomTopIndex];
            if (tileNumber <= tileTextures.size() && tileTextures[tileNumber - 1])
            {
                topTile.tileNumber = tileNumber;
                topTile.sprite.setTexture(*tileTextures[tileNumber - 1]);
                topTile.sprite.setPosition(rightmostX, 720 - (gridHeight * tileSize));
                topTile.isTop = true;
                newColumn.push_back(topTile);
            }

            // Fill below with random filler tiles
            for (int y = 1; y < gridHeight; y++)
            {
                GroundTile fillerTile;
                int randomFillerIndex = rand() % fillerTiles.size();
                int fillerTileNumber = fillerTiles[randomFillerIndex];
                if (fillerTileNumber <= tileTextures.size() && tileTextures[fillerTileNumber - 1])
                {
                    fillerTile.tileNumber = fillerTileNumber;
                    fillerTile.sprite.setTexture(*tileTextures[fillerTileNumber - 1]);
                    fillerTile.sprite.setPosition(rightmostX, 720 - ((gridHeight - y) * tileSize));
                    fillerTile.isTop = false;
                    newColumn.push_back(fillerTile);
                }
            }

            tiles.push_back(newColumn);
        }
    }

    void draw(RenderWindow &window)
    {
        for (const auto &column : tiles)
        {
            for (const auto &tile : column)
            {
                window.draw(tile.sprite);
            }
        }
    }
};

int main()
{
    cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    RenderWindow window(VideoMode(1280, 720), "SFML");
    window.setVerticalSyncEnabled(true);
    srand(time(0));

    // Define relative paths
    string assetsPath = "../assets";
    string worldPath = assetsPath + "/world";
    string spritesPath = assetsPath + "/sprites";
    string groundPath = worldPath + "/ground";

    // Load textures
    vector<RectangleShape> backgrounds;
    Texture backgroundTexture, cloud1Texture, cloud2Texture;
    Texture runTexture, idleTexture, attackTexture;

    // Create Ground instance with relative path
    Ground ground(groundPath);

    // Load all required textures with error checking
    bool loadSuccess = true;

    if (!runTexture.loadFromFile(spritesPath + "/run.png"))
    {
        cerr << "Failed to load run texture!" << endl;
        loadSuccess = false;
    }

    if (!idleTexture.loadFromFile(spritesPath + "/idle.png"))
    {
        cerr << "Failed to load idle texture!" << endl;
        loadSuccess = false;
    }

    if (!attackTexture.loadFromFile(spritesPath + "/attack.png"))
    {
        cerr << "Failed to load attack texture!" << endl;
        loadSuccess = false;
    }

    if (!backgroundTexture.loadFromFile(worldPath + "/background.png"))
    {
        cerr << "Failed to load background texture!" << endl;
        loadSuccess = false;
    }

    if (!cloud1Texture.loadFromFile(worldPath + "/cloud1.png"))
    {
        cerr << "Failed to load cloud1 texture!" << endl;
        loadSuccess = false;
    }

    if (!cloud2Texture.loadFromFile(worldPath + "/cloud2.png"))
    {
        cerr << "Failed to load cloud2 texture!" << endl;
        loadSuccess = false;
    }

    if (!loadSuccess)
    {
        cerr << "Failed to load one or more textures!" << endl;
        return -1;
    }

    // Create three background instances for seamless scrolling
    for (int i = 0; i < 3; i++)
    {
        RectangleShape background(Vector2f(1280, 720));
        background.setTexture(&backgroundTexture);
        background.setPosition(i * 1280 - 1, 0);
        backgrounds.push_back(background);
    }

    float scrollSpeed = 50.0f;
    float totalWidth = 1280.0f * 3; // Total width of all background instances

    vector<Cloud> clouds;
    initializeClouds(clouds, cloud1Texture, cloud2Texture, scrollSpeed, totalWidth);

    Sprite sprite;
    sprite.setTexture(idleTexture);

    Vector2f position(400, 300);
    int frameWidth = 128, frameHeight = 128;
    int currentFrame = 0, attackFrame = 0;
    bool facingRight = true, isRunning = false, isAttacking = false;
    Clock movementClock, attackClock, gameClock;
    int totalAttackFrames = 4;

    while (window.isOpen())
    {
        float deltaTime = gameClock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        // Update background scrolling
        for (auto &background : backgrounds)
        {
            background.move(-scrollSpeed * deltaTime, 0);
            if (background.getPosition().x <= -1280)
            {
                float rightmostX = -1280.0f;
                for (const auto &bg : backgrounds)
                {
                    rightmostX = max(rightmostX, bg.getPosition().x);
                }
                background.setPosition(rightmostX + 1280 - 1, 0);
            }
        }

        updateClouds(clouds, deltaTime, cloud1Texture, cloud2Texture, scrollSpeed);
        // Inside the main game loop
        ground.update(scrollSpeed * deltaTime);

        attack(sprite, attackTexture, attackFrame, attackClock, isAttacking, isRunning, facingRight, position, totalAttackFrames);
        movement(position, sprite, runTexture, idleTexture, currentFrame, facingRight, movementClock, isRunning, isAttacking);

        if (!isAttacking)
        {
            sprite.setTexture(isRunning ? runTexture : idleTexture);
            sprite.setTextureRect(IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
        }

        if (!facingRight)
        {
            sprite.setScale(-2.f, 2.f);
            sprite.setOrigin(frameWidth, 0);
        }
        else
        {
            sprite.setScale(2.f, 2.f);
            sprite.setOrigin(0, 0);
        }

        sprite.setPosition(position);

        window.clear();

        // Draw layers in correct order
        // Draw layers in correct order
        for (const auto &background : backgrounds)
        {
            window.draw(background);
        }

        for (const auto &cloud : clouds)
        {
            window.draw(cloud.sprite);
        }

        ground.draw(window); // Add this line
        window.draw(sprite);

        window.display();
    }

    return 0;
}