#pragma once

#include "entities/traffic/TrafficVehicle.h"

class Sedan : public TrafficVehicle
{
public:
    Sedan(float startY, int startLane);

    void Draw() const override;
};