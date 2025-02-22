#include "ground.hpp"
#include "player.hpp"

Ground::Ground(const string &basePath)
{
    cout << "Initializing Ground with base path: " << basePath << endl;

    // Load all tile textures
    for (int i = 1; i <= 96; i++)
    {
        auto texture = make_unique<Texture>();
        string filename = basePath + "/Tile_" + (i < 10 ? "0" : "") + to_string(i) + ".png";

        if (!texture->loadFromFile(filename))
        {
            cerr << "Failed to load texture: " << filename << endl;
            if (!std::filesystem::exists(filename))
            {
                cerr << "File does not exist at path: " << filename << endl;
            }
            tileTextures.push_back(make_unique<Texture>());
            continue;
        }
        cout << "Successfully loaded: " << filename << endl;
        tileTextures.push_back(move(texture));
    }

    cout << "Total textures loaded: " << tileTextures.size() << endl;
    lastObstacleX = 0;
    minObstacleSpacing = 300.0f; // Minimum pixels between obstacles
    gameTime = 0.0f;
    initialDelay = 5.0f;
    generateInitialGround();
}

void Ground::generateInitialGround()
{
    int initialColumns = (1280 / tileSize) + 2;

    for (int x = 0; x < initialColumns; x++)
    {
        vector<GroundTile> column;

        // Generate top tile
        GroundTile topTile;
        int tileNumber = topTiles[rand() % topTiles.size()];
        if (tileNumber <= tileTextures.size() && tileTextures[tileNumber - 1])
        {
            topTile.tileNumber = tileNumber;
            topTile.sprite.setTexture(*tileTextures[tileNumber - 1]);
            topTile.sprite.setPosition(x * tileSize, 720 - (gridHeight * tileSize));
            topTile.isTop = true;
            column.push_back(topTile);
        }

        // Generate filler tiles
        for (int y = 1; y < gridHeight; y++)
        {
            GroundTile fillerTile;
            int fillerTileNumber = fillerTiles[rand() % fillerTiles.size()];
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

void Ground::spawnObstacleGroup(float xPosition)
{
    // Decide whether to spawn a single or stacked obstacle
    bool spawnStacked = (rand() / static_cast<float>(RAND_MAX) < 0.3f); // 30% chance for stacked obstacles

    // Spawn bottom obstacle
    Obstacle bottomObstacle;
    int obstacleType = obstacleTextureIds[rand() % obstacleTextureIds.size()];

    if (obstacleType <= tileTextures.size() && tileTextures[obstacleType - 1])
    {
        bottomObstacle.sprite.setTexture(*tileTextures[obstacleType - 1]);
        bottomObstacle.sprite.setPosition(xPosition, 720 - (gridHeight * tileSize) - tileSize);
        bottomObstacle.isActive = true;
        bottomObstacle.hitbox = bottomObstacle.sprite.getGlobalBounds();
        obstacles.push_back(bottomObstacle);

        // Possibly spawn a second obstacle on top
        if (spawnStacked)
        {
            Obstacle topObstacle;
            int topObstacleType = obstacleTextureIds[rand() % obstacleTextureIds.size()];

            if (topObstacleType <= tileTextures.size() && tileTextures[topObstacleType - 1])
            {
                topObstacle.sprite.setTexture(*tileTextures[topObstacleType - 1]);
                // Position the top obstacle above the bottom one
                topObstacle.sprite.setPosition(xPosition,
                                               bottomObstacle.sprite.getPosition().y - tileSize);
                topObstacle.isActive = true;
                topObstacle.hitbox = topObstacle.sprite.getGlobalBounds();
                obstacles.push_back(topObstacle);
            }
        }
    }
}

void Ground::handlePlayerCollision(Player &player)
{

    FloatRect playerHitbox = player.sprite.getGlobalBounds();
    // Adjust these values based on your player sprite to create a tighter box
    float hitboxScale = 0.1f; // Reduce hitbox to 60% of sprite size

    // Calculate the reduced hitbox dimensions while keeping it centered
    float widthReduction = playerHitbox.width * (1 - hitboxScale);
    // float heightReduction = playerHitbox.height * (1 - hitboxScale);

    FloatRect tightPlayerHitbox(
        playerHitbox.left + (widthReduction / 2), // Center the hitbox horizontally
        playerHitbox.top,                         // Center the hitbox vertically
        playerHitbox.width * hitboxScale,         // Reduced width
        playerHitbox.height                       // Reduced height
    );

    for (auto &obstacle : obstacles)
    {
        if (obstacle.isActive && obstacle.hitbox.intersects(tightPlayerHitbox))
        {
            if (player.jumpVelocity > 0) // Player is jumping
            {
                // Player successfully avoids the obstacle
                continue;
            }
            else
            {
                // Player collides with the obstacle
                player.isColliding = true;
                player.pushBackAmount = 2.0f;
                cout << "Collision detected! Game Over." << endl;
                return;
            }
        }
    }
}

void Ground::update(float deltaTime, float scrollAmount, Player &player)
{
    // Update game time
    gameTime += deltaTime;

    // Update ground tiles
    for (auto &column : tiles)
    {
        for (auto &tile : column)
        {
            tile.sprite.move(-scrollAmount, 0);
        }
    }

    // Update obstacles
    for (auto &obstacle : obstacles)
    {
        obstacle.sprite.move(-scrollAmount, 0);
        obstacle.hitbox = obstacle.sprite.getGlobalBounds();
    }

    // Update last obstacle position
    lastObstacleX -= scrollAmount;

    // Remove off-screen obstacles
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
                       [](const Obstacle &obs)
                       { return obs.sprite.getPosition().x < -100; }),
        obstacles.end());

    // Generate new tiles when needed
    if (!tiles.empty() && !tiles[0].empty() && tiles[0][0].sprite.getPosition().x < -tileSize)
    {
        tiles.erase(tiles.begin());

        vector<GroundTile> newColumn;
        float rightmostX = tiles.back()[0].sprite.getPosition().x + tileSize;

        // Generate new top tile
        GroundTile topTile;
        int tileNumber = topTiles[rand() % topTiles.size()];
        if (tileNumber <= tileTextures.size() && tileTextures[tileNumber - 1])
        {
            topTile.tileNumber = tileNumber;
            topTile.sprite.setTexture(*tileTextures[tileNumber - 1]);
            topTile.sprite.setPosition(rightmostX, 720 - (gridHeight * tileSize));
            topTile.isTop = true;
            newColumn.push_back(topTile);
        }

        // Generate new filler tiles
        for (int y = 1; y < gridHeight; y++)
        {
            GroundTile fillerTile;
            int fillerTileNumber = fillerTiles[rand() % fillerTiles.size()];
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

        // Only spawn obstacles after initial delay
        if (gameTime >= initialDelay)
        {
            if (rightmostX - lastObstacleX >= minObstacleSpacing &&
                rand() / static_cast<float>(RAND_MAX) < obstacleSpawnChance)
            {
                spawnObstacleGroup(rightmostX);
                lastObstacleX = rightmostX;
            }
        }
    }

    // Handle player collision
    handlePlayerCollision(player);
}

void Ground::draw(RenderWindow &window)
{
    // Draw ground tiles
    for (const auto &column : tiles)
    {
        for (const auto &tile : column)
        {
            window.draw(tile.sprite);
        }
    }

    // Draw obstacles
    for (const auto &obstacle : obstacles)
    {
        if (obstacle.isActive)
        {
            window.draw(obstacle.sprite);
        }
    }
}
