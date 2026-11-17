#pragma once

#include <array>

class Road
{
private:
    float dashOffset;
    float elapsed;

public:
    Road();
    void Update(float deltaTime, float scrollSpeed);
    void Draw() const;

    float GetLeft() const;
    float GetRight() const;
    float GetWidth() const;
    float GetLaneWidth() const;
    std::array<float, 4> GetLaneCenters() const;
};
