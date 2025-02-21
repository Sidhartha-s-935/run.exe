#include "cloud.hpp"
#include <cstdlib>

Cloud::Cloud(const Texture &texture, float baseSpeed, float initialX)
{
    sprite.setTexture(texture);
    float scale = 0.8f + (rand() % 30) / 100.0f;
    sprite.setScale(scale, scale);

    float maxHeight = 720 * 0.9f;
    float minHeight = 50;
    float height = minHeight + (rand() % int(maxHeight - minHeight));

    sprite.setPosition(initialX, height);
    speed = baseSpeed * (0.9f + (rand() % 20) / 100.0f);
}

Cloud::Cloud(const Texture &texture, float baseSpeed) : Cloud(texture, baseSpeed, 1280.0f) {}

void Cloud::reset(float screenWidth)
{
    float maxHeight = 720 * 0.75f;
    float minHeight = 50;
    sprite.setPosition(
        screenWidth + sprite.getGlobalBounds().width,
        minHeight + (rand() % int(maxHeight - minHeight)));
}

void initializeClouds(vector<Cloud> &clouds, const Texture &cloud1Texture,
                      const Texture &cloud2Texture, float scrollSpeed, float totalWidth)
{
    int numClouds = 12;
    float cloudSpacing = totalWidth / numClouds;

    for (int i = 0; i < numClouds; i++)
    {
        float xPos = i * cloudSpacing + (rand() % int(cloudSpacing * 0.5f));
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

    const int desiredCloudCount = 12;
    if (clouds.size() < desiredCloudCount)
    {
        clouds.push_back(Cloud(rand() % 2 == 0 ? cloud1Texture : cloud2Texture, scrollSpeed));
    }
}
