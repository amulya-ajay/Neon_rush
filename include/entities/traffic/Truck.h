#pragma once

#include "entities/traffic/TrafficVehicle.h"

class Truck : public TrafficVehicle
{
public:
    Truck(float startY, int startLane);

    void Draw() const override;
};