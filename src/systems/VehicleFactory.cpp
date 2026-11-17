#include "systems/VehicleFactory.h"

#include "entities/traffic/PoliceCar.h"
#include "entities/traffic/Sedan.h"
#include "entities/traffic/SportsCar.h"
#include "entities/traffic/Truck.h"

std::unique_ptr<TrafficVehicle> VehicleFactory::Create(TrafficType type, float startY, int lane)
{
    switch (type)
    {
    case TrafficType::Sedan:
        return std::make_unique<Sedan>(startY, lane);
    case TrafficType::SportsCar:
        return std::make_unique<SportsCar>(startY, lane);
    case TrafficType::Truck:
        return std::make_unique<Truck>(startY, lane);
    case TrafficType::PoliceCar:
        return std::make_unique<PoliceCar>(startY, lane);
    }
    return std::make_unique<Sedan>(startY, lane);
}
