#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <stdlib.h>
#include <cmath>
#include "Agent.h"
#include "Random.hpp"

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

#define TILE_GROWTH 1LL
#define FOOD_RARITY 1
#define ENERGY_ABSORPTION_RATE 3
#define MAX_ENERGY_IN_GENERATED_TILE 100

#define POPULATION_MINIMUM 200

#define INITIAL_MUTATION_CHANCE 0.1f
#define CHANCE_OF_MUTATED_COPY 0.2f
#define MUTATION_CHANCE_ON_COPY 0.2f
#define MUTATION_APLITUDE 0.2f
#define COLOR_CHANCE_CHANCE 0.1f

#define WALKING_COST 2
#define BIRTH_COST 40
#define AGENT_MINIMUM_ENERGY 0
#define AGENT_MAXIMUM_ENERGY 100
#define AGENT_STARTING_ENERGY 20
#define AGENT_PASSIVE_COST 2

#define ATTACK_COST 10
#define COST_WHEN_ATTACKED 30

#define ALLOW_OVERCROUD_COST true
#define OVER_2_NEIGHBOURS_COST 5

#define USE_AUTOMATIC_SELECTION false // Experimental !!!!

class Simulation {
public:

	const int MapSize = 500;

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
	bool AttackAtPosition(sf::Vector2i Position);
	void PenaliseForPossibleNeighbourLimit(Agent* agent);
	bool IsPositionValid(sf::Vector2i Position);
	float* CompileAgentInput(Agent* agent);
	sf::Color GenerateColor();
	int GetWorldArea();
	TileData GetTileDataAt(int x, int y);
};