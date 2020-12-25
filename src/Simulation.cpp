#include <SFML/Graphics.hpp>
#include <vector>
#include "Agent.h"
#include "Simulation.h"

void Simulation::Init() {
	// Allocating the Map
}

void Simulation::UpdateSimulation() {
	int AgentCount = Agents.size();
	for (int i = 0; i < AgentCount;i++) {
		Agents[i]->Update();
	}
}

void Simulation::AddAgent(sf::Color Color, sf::Vector2f Position) {
	Agents.push_back(new Agent(Color, Position));
}