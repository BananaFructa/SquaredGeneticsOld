#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

sf::RenderWindow Window(sf::VideoMode(800, 800), "Squared Genetics");

#include "Simulation.h"

Simulation MainSimulation;

#include "RenderManager.h"
#include "Vendor/Timer.h"
Timer UpdateTimer;

int main() {

    srand(456);

    MainSimulation.Init();

    std::thread InputThread([=]() {
        for (;;) {
            char* line = new char[256];
            std::cin.getline(line,256);
            float value = std::atof(line);
            MainSimulation.MutexRenderUpdateLoop.lock();
            MainSimulation.GenerateFoodMap(Constants::FOOD_RARITY, value);
            Constants::MAX_ENERGY_IN_GENERATED_TILE = value;
            MainSimulation.MutexRenderUpdateLoop.unlock();
            delete[] line;
        }
    });

    InputThread.detach();

    UpdateTimer.SetInterval([]() {
        MainSimulation.UpdateSimulation();
    },0);

    RenderManager::SetWindow(&Window);
    RenderManager::SetSimulation(&MainSimulation);

    RenderManager::RenderLoop();

    return 0;
}


