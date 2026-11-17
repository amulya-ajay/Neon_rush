#pragma once

#include "entities/traffic/TrafficVehicle.h"

#include <memory>

enum class TrafficType
{
    Sedan,
    SportsCar,
    Truck,
    PoliceCar
};

class VehicleFactory
{
public:
    static std::unique_ptr<TrafficVehicle> Create(TrafficType type, float startY, int lane);
};
