#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include "Simulation.h"
#include "Vendor/Timer.h"

class RenderManager {

private:

	static std::mutex MutexLock;
	static Timer EnergyMapUpdate;
	static sf::Vector2f LastCameraPosition;
	static int XUnitsInFrame;
	static int YUnitsInFrame;
	static void CameraChangedUnit();
	static void FOVChanged();
	static int AreaInFrame;


public:

	static sf::RenderWindow* Window;
	static sf::Vector2f CameraPosition;
	static float FieldOfView;
	static Simulation* SimulationToRender;

	static sf::VertexArray FoodMapArray;

	static bool IsGridDisplayed;

	static void SetWindow(sf::RenderWindow* Window);
	static void SetSimulation(Simulation* Simulation);
	static void RenderLoop();
	static void RunEvent(sf::Event Event);
	static void GenerateFoodMapMesh();
	static void UpdateFoodMapColors();

};