#pragma once

class CityBackground
{
private:
    float elapsed;

public:
    CityBackground();
    void Update(float deltaTime);
    void Draw(bool titleMode = false) const;
};
