#include "ground.hpp"

Ground::Ground(const string &basePath)
{
    cout << "Initializing Ground with base path: " << basePath << endl;

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
    generateInitialGround();
}

void Ground::generateInitialGround()
{
    int initialColumns = (1280 / tileSize) + 2;

    for (int x = 0; x < initialColumns; x++)
    {
        vector<GroundTile> column;

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

void Ground::update(float scrollAmount)
{
    for (auto &column : tiles)
    {
        for (auto &tile : column)
        {
            tile.sprite.move(-scrollAmount, 0);
        }
    }

    if (!tiles.empty() && !tiles[0].empty() && tiles[0][0].sprite.getPosition().x < -tileSize)
    {
        tiles.erase(tiles.begin());

        vector<GroundTile> newColumn;
        float rightmostX = tiles.back()[0].sprite.getPosition().x + tileSize;

        // Generate top tile
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
    }
}

void Ground::draw(RenderWindow &window)
{
    for (const auto &column : tiles)
    {
        for (const auto &tile : column)
        {
            window.draw(tile.sprite);
        }
    }
}
