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
                  const Texture &cloud2Texture, const Texture &cloud3Texture, const Texture &cloud4Texture, float scrollSpeed, int score)
{
    bool useNightClouds = (score / 2000) % 2 == 1;

    for (auto &cloud : clouds)
    {
        cloud.sprite.move(-cloud.speed * deltaTime, 0);

        if (cloud.sprite.getPosition().x < -cloud.sprite.getGlobalBounds().width)
        {
            cloud.reset(1280.0f);

            // When resetting a cloud, use the appropriate texture set
            if (useNightClouds)
            {
                cloud.sprite.setTexture(rand() % 2 == 0 ? cloud3Texture : cloud4Texture);
            }
            else
            {
                cloud.sprite.setTexture(rand() % 2 == 0 ? cloud1Texture : cloud2Texture);
            }

            // Adjust opacity for smooth transition near threshold
            int scoreInCycle = score % 2000;
            if (scoreInCycle > 1900 || scoreInCycle < 100)
            {
                float alpha = scoreInCycle > 1900 ? (scoreInCycle - 1900) / 100.0f : 1.0f - (scoreInCycle / 100.0f);
                cloud.sprite.setColor(Color(255, 255, 255, static_cast<uint8_t>(255 * alpha)));
            }
            else
            {
                cloud.sprite.setColor(Color(255, 255, 255, 255));
            }
        }
    }

    const int desiredCloudCount = 12;
    if (clouds.size() < desiredCloudCount)
    {
        if (useNightClouds)
        {
            clouds.push_back(Cloud(rand() % 2 == 0 ? cloud3Texture : cloud4Texture, scrollSpeed));
        }
        else
        {
            clouds.push_back(Cloud(rand() % 2 == 0 ? cloud1Texture : cloud2Texture, scrollSpeed));
        }
    }
}
