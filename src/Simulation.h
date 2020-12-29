#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <stdlib.h>
#include <cmath>
#include "Agent.h"

struct TileData {
	short Energy;
	short EnergyCapacity;
	bool AgentIsAttacked;
	bool AgentAttacks;
	bool Signal;
	float R;
	float G;
	float B;
};

struct TileComponent {
	short Energy;
	short MaxEnergy;
	Agent* Agent = nullptr;
};

#define MAX_12BIT 4095
#define MAX_10BIT 1023
#define MAX_ENERGY_IN_GENERATED_TILE 100

#define INITIAL_MUTATION_CHANCE 0.3f
#define INITIAL_MUTATION_AMPLITUDE 1.0f
#define CHANCE_OF_MUTATED_COPY 0.3f
#define MUTATION_CHANCE_ON_COPY 0.2f
#define MUTATION_APLITUDE_ON_COPY 0.5f

#define TILE_GROWTH 1LL

#define WALKING_COST 2
#define BIRTH_COST 40
#define AGENT_MINIMUM_ENERGY 0
#define AGENT_MAXIMUM_ENERGY 300
#define AGENT_STARTING_ENERGY 20
#define AGENT_PASSIVE_COST 2

#define ENERGY_ABSORPTION_RATE 3

#define COLOR_CHANCE_CHANCE 0.1f

#define POPULATION_MINIMUM 70

#define FOOD_RARITY 4

#define USE_AUTOMATIC_SELECTION false // Experimental !!!!

class Simulation {
public:

	const int MapSize = 100;

	std::vector<Agent*> Agents;
	std::mutex MutexRenderUpdateLoop;

	TileComponent** Map;

	void Init();
	void UpdateSimulation();
	void AddAgent(sf::Color Color, sf::Vector2i Position);
	bool AddAgentFromCopy(Agent* agent);
	void DestroyAgent(Agent* agent);
	void SetTileToAgentData(Agent* agent);
	bool SetAgentPosition(Agent* agent, sf::Vector2i Position);
	void AgentConsumesEnergy(Agent* agent);
	float* CompileAgentInput(Agent* agent);
	sf::Color GenerateColor();
	int GetWorldArea();
	TileData GetTileDataAt(int x, int y);
};