#pragma once

#include "entities/traffic/TrafficVehicle.h"

class SportsCar : public TrafficVehicle
{
public:
    SportsCar(float startY, int startLane);

    void Draw() const override;
};
