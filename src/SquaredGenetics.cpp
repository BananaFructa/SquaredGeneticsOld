#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

sf::RenderWindow Window(sf::VideoMode(800, 800), "Squared Genetics");

#include "Timer.h"
#include "Agent.h"
#include "Simulation.h"

float FieldOfView = 10;
sf::Vector2f CameraPosition(0, 0);

#include "RenderUtils.h"
#include "NN/FFNN.h"

Timer UpdateTimer;
Simulation MainSimulation;

void RenderLoop();

int main() {

    srand(std::chrono::system_clock::now().time_since_epoch().count());

    MainSimulation.AddAgent(sf::Color::Color(255, 0, 0),sf::Vector2f(0,0));

    int sizes[] = { 465,150,100,56 };
    AFunction yes[] = { AFunctions::LeakyReLU, AFunctions::LeakyReLU, AFunctions::Tanh };

    FFNN test(sizes,yes,4);
    float input[465];
    for (int i = 0;i < 100000;i++) {
        test.Predict(input);
        if (i%1000==0)std::cout << i << '\n';
    }
    std::cout << test.Predict(input)[2];

    UpdateTimer.SetInterval([]() {
        MainSimulation.UpdateSimulation();
    },16);

    RenderLoop();

    return 0;
}

void RenderLoop() {
    Window.setFramerateLimit(60);

    while (Window.isOpen()) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            CameraPosition.y -= 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            CameraPosition.x -= 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            CameraPosition.y += 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            CameraPosition.x += 0.1f;
        }

        sf::Event event;

        NormalizeViewport(FieldOfView, CameraPosition);

        while (Window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                Window.close();

            if (event.type == sf::Event::MouseWheelMoved) {
                if (event.mouseWheel.delta > 0) FieldOfView++;
                if (event.mouseWheel.delta < 0) FieldOfView--;
            }
        }

        Window.clear();

        DrawGrid(FieldOfView);
        
        int AgentCount = MainSimulation.Agents.size();
        for (int i = 0;i < AgentCount;++i) {
            Window.draw(MainSimulation.Agents[i]->Shape);
        }

        Window.display();
    }
}


