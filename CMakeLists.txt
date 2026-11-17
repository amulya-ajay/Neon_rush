cmake_minimum_required(VERSION 3.25)

project(NEON_RUSH LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib CONFIG REQUIRED)

add_executable(NEON_RUSH
    src/main.cpp
    src/game/Game.cpp
    src/world/Road.cpp
    src/world/CityBackground.cpp
    src/entities/Vehicle.cpp
    src/entities/PlayerCar.cpp
    src/entities/Collectible.cpp
    src/entities/traffic/TrafficVehicle.cpp
    src/entities/traffic/Sedan.cpp
    src/entities/traffic/Truck.cpp
    src/entities/traffic/SportsCar.cpp
    src/entities/traffic/PoliceCar.cpp
    src/systems/VehicleFactory.cpp
    src/systems/TrafficManager.cpp
    src/systems/CollectibleManager.cpp
    src/systems/ParticleSystem.cpp
    src/systems/ScoreManager.cpp
    src/systems/SaveManager.cpp
    src/systems/AudioManager.cpp
)

target_include_directories(NEON_RUSH PRIVATE include)
target_link_libraries(NEON_RUSH PRIVATE raylib)

include(CTest)
if(BUILD_TESTING)
    add_executable(NEON_RUSH_LOGIC_TESTS
        tests/logic_tests.cpp
        src/entities/Vehicle.cpp
        src/entities/PlayerCar.cpp
        src/entities/traffic/TrafficVehicle.cpp
        src/entities/traffic/Sedan.cpp
        src/entities/traffic/SportsCar.cpp
        src/entities/traffic/Truck.cpp
        src/entities/traffic/PoliceCar.cpp
        src/systems/VehicleFactory.cpp
        src/systems/TrafficManager.cpp
        src/systems/ScoreManager.cpp
        src/systems/SaveManager.cpp
    )
    target_include_directories(NEON_RUSH_LOGIC_TESTS PRIVATE include)
    target_link_libraries(NEON_RUSH_LOGIC_TESTS PRIVATE raylib)
    add_test(NAME neon_rush_logic COMMAND NEON_RUSH_LOGIC_TESTS)
endif()
