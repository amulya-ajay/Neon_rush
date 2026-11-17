#pragma once

#include "entities/traffic/TrafficVehicle.h"

class PoliceCar : public TrafficVehicle
{
public:
    PoliceCar(float startY, int startLane);
    void Draw() const override;
};
