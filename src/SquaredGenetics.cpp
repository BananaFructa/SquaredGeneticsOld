#include <iostream>
#include <thread>
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

    UpdateTimer.SetInterval([]() {
        MainSimulation.UpdateSimulation();
    },0);

    RenderManager::SetWindow(&Window);
    RenderManager::SetSimulation(&MainSimulation);

    RenderManager::RenderLoop();

    return 0;
}


