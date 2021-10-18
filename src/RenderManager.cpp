#include <thread>
#include "RenderManager.h"
#include "RenderUtils.hpp"

std::mutex RenderManager::MutexLock;
Timer RenderManager::EnergyMapUpdate;
sf::RenderWindow* RenderManager::Window;
sf::Vector2f RenderManager::CameraPosition = sf::Vector2f(0, 0);
sf::Vector2f RenderManager::LastCameraPosition = sf::Vector2f(0, 0);
float RenderManager::FieldOfView = 10;
Simulation* RenderManager::SimulationToRender;
bool RenderManager::IsGridDisplayed = true;
int RenderManager::XUnitsInFrame;
int RenderManager::YUnitsInFrame;
bool RenderManager::SignalMapMode;
bool RenderManager::AttackMapMode;

sf::VertexArray RenderManager::FoodMapArray;

void RenderManager::SetWindow(sf::RenderWindow* Window) {
	RenderManager::Window = Window;
}

void RenderManager::SetSimulation(Simulation* Simulation) {
    RenderManager::SimulationToRender = Simulation;
}

void RenderManager::FOVChanged() {
    XUnitsInFrame = XUnitsCount(Window,FieldOfView);
    YUnitsInFrame = YUnitsCount(Window, FieldOfView);
}

void RenderManager::RenderLoop() {

    FOVChanged();
    GenerateFoodMapMesh();
    UpdateFoodMapColors();

    Timer EnergyMapUpdate;

    EnergyMapUpdate.SetInterval([]() {
        MutexLock.lock();
        UpdateFoodMapColors();
        MutexLock.unlock();
    }, 1000);

    Window->setFramerateLimit(30);

    while (Window->isOpen()) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            CameraPosition.y -= 2.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            CameraPosition.x -= 2.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            CameraPosition.y += 2.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            CameraPosition.x += 2.0f;
        }

        LastCameraPosition = CameraPosition;

        sf::Event event;

        while (Window->pollEvent(event)) {
            RunEvent(event);
        }

        Window->clear();

        NormalizeViewport(Window, FieldOfView, CameraPosition);

        MutexLock.lock();
        Window->draw(FoodMapArray);
        MutexLock.unlock();

        SimulationToRender->MutexRenderUpdateLoop.lock();
        int AgentCount = SimulationToRender->Agents.size();
        for (int i = 0;i < AgentCount;++i) {
            if (!SignalMapMode && !AttackMapMode) {
                Window->draw(SimulationToRender->Agents[i]->Shape);
            }
            else if (SignalMapMode){
                if (SimulationToRender->Agents[i]->CurrentSignalState) {
                    sf::RectangleShape rect(sf::Vector2f(1.0f, 1.0f));
                    rect.setFillColor(sf::Color::Color(255, 255, 0));
                    rect.setPosition((sf::Vector2f)SimulationToRender->Agents[i]->Position);
                    Window->draw(rect);
                }
            }
            else if (AttackMapMode) {
                sf::RectangleShape rect(sf::Vector2f(1.0f, 1.0f));
                rect.setFillColor(sf::Color::Color(
                    255 * SimulationToRender->Agents[i]->Attacks,
                    0, 255 * SimulationToRender->Agents[i]->IsAttacked,
                    255 * (SimulationToRender->Agents[i]->Attacks || SimulationToRender->Agents[i]->IsAttacked
                )));
                rect.setPosition((sf::Vector2f)SimulationToRender->Agents[i]->Position);
                Window->draw(rect);
            }
        }
        SimulationToRender->MutexRenderUpdateLoop.unlock();

        NormalizeViewport(Window, FieldOfView, sf::Vector2f(Fract(CameraPosition.x), Fract(CameraPosition.y)));

        IsGridDisplayed = FieldOfView > 7;

        if (IsGridDisplayed)
            DrawGrid(Window, FieldOfView);

        Window->display();
    }
}

void RenderManager::RunEvent(sf::Event Event) {
    if (Event.type == sf::Event::Closed)
        Window->close();

    if (Event.type == sf::Event::MouseWheelMoved) {
        if (Event.mouseWheel.delta > 0) FieldOfView++;
        if (Event.mouseWheel.delta < 0) FieldOfView--;
        FOVChanged();
    }

    if (Event.type == sf::Event::KeyPressed) {
        if (Event.key.code == sf::Keyboard::P) SignalMapMode = !SignalMapMode;
        if (Event.key.code == sf::Keyboard::O) AttackMapMode = !AttackMapMode;
    }
}

void RenderManager::GenerateFoodMapMesh() {
    FoodMapArray = sf::VertexArray(sf::Quads, 4 * SimulationToRender->MapSize * SimulationToRender->MapSize);

    int index = 0;

    int xV = SimulationToRender->MapSize / 2;
    int yV = SimulationToRender->MapSize / 2;

    for (int x = -xV; x < xV; ++x) {
        for (int y = -yV; y < yV; ++y) {
            FoodMapArray[index++] = sf::Vector2f(x, y);
            FoodMapArray[index++] = sf::Vector2f(x+1, y);
            FoodMapArray[index++] = sf::Vector2f(x+1, y+1);
            FoodMapArray[index++] = sf::Vector2f(x , y+1);
        }
    }
}

void RenderManager::UpdateFoodMapColors() {

    int index = 0;
    for (int i = 0; i < SimulationToRender->MapSize; ++i) {
        for (int j = 0; j < SimulationToRender->MapSize; ++j) {
            float factor = (float)((SimulationToRender->Map[i][j].Energy) / (float)Constants::MAX_ENERGY_IN_GENERATED_TILE);
            sf::Color Color = sf::Color::Color(13 * factor, 140 * factor, 5 * factor);
            float x = i - SimulationToRender->MapSize / 2;
            float y = j - SimulationToRender->MapSize / 2;
            FoodMapArray[index++].color = Color;
            FoodMapArray[index++].color = Color;
            FoodMapArray[index++].color = Color;
            FoodMapArray[index++].color = Color;
        }
    }
}
