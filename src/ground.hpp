#ifndef GROUND_HPP
#define GROUND_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace sf;

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
    const vector<int> fillerTiles = {4, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20};

    const int tileSize = 32;
    const int gridHeight = 5;
    int currentLeftmostTileIndex = 0;

public:
    Ground(const string &basePath);
    void generateInitialGround();
    void update(float scrollAmount);
    void draw(RenderWindow &window);
};

#endif // GROUND_HPP
