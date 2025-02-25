#ifndef CLOUD_HPP
#define CLOUD_HPP

#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

struct Cloud
{
    Sprite sprite;
    float speed;
    Cloud(const Texture &texture, float baseSpeed, float initialX);
    Cloud(const Texture &texture, float baseSpeed);

    void reset(float screenWidth);
};

void initializeClouds(vector<Cloud> &clouds, const Texture &cloud1Texture,
                      const Texture &cloud2Texture, float scrollSpeed, float totalWidth);

void updateClouds(std::vector<Cloud> &clouds, float deltaTime, const Texture &cloud1Texture, const Texture &cloud2Texture,
                  const Texture &cloud3Texture, const Texture &cloud4Texture, float scrollSpeed, int score);

#endif // CLOUD_HPP
