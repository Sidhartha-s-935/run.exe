#ifndef GROUND_HPP
#define GROUND_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>
#include "player.hpp"

using namespace std;
using namespace sf;

struct GroundTile
{
    Sprite sprite;
    int tileNumber;
    bool isTop;
};

struct Obstacle
{
    Sprite sprite;
    bool isActive;
    FloatRect hitbox;
};

class Ground
{
private:
    vector<vector<GroundTile>> tiles;
    vector<unique_ptr<Texture>> tileTextures;

    // Tile selection vectors
    const vector<int> topTiles = {1, 2, 3, 5, 6, 7, 8, 18, 33, 34, 49, 50, 51, 54, 55};
    const vector<int> fillerTiles = {4, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20};

    // Obstacle management
    vector<Obstacle> obstacles;
    const vector<int> obstacleTextureIds = {22, 23, 30, 33, 35}; // Dangerous-looking tiles
    const float obstacleSpawnChance = 0.3f;

    // Configuration
    const int tileSize = 32;
    const int gridHeight = 5;
    // Helper method
    void spawnObstacleGroup(float xPosition);

public:
    Ground(const string &basePath);
    void generateInitialGround();
    bool checkCollision(const FloatRect &playerBounds);
    void handlePlayerCollision(Player &player);
    void update(float deltaTime, float scrollAmount, Player &player);
    void draw(RenderWindow &window);

    float lastObstacleX;
    float minObstacleSpacing;
    float gameTime;
    float initialDelay;
};

#endif // GROUND_HPP