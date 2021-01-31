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
	int R;
	int G;
	int B;
};

struct TileComponent {
	short Energy;
	short MaxEnergy;
	Agent* Agent = nullptr;
};

namespace Constants {

	extern long long TILE_GROWTH;
	extern int FOOD_RARITY;
	extern int ENERGY_ABSORPTION_RATE;
	extern int MAX_ENERGY_IN_GENERATED_TILE;

	extern int POPULATION_MINIMUM;

	extern float INITIAL_MUTATION_APLITUDE;
	extern float INITIAL_MUTATION_CHANCE;
	extern float CHANCE_OF_MUTATED_COPY;
	extern float MUTATION_APLITUDE;
	extern float MUTATION_CHANCE_ON_COPY;
	extern float COLOR_CHANCE_CHANCE;

	extern int WALKING_COST;
	extern int BIRTH_COST;
	extern int AGENT_MINIMUM_ENERGY;
	extern int AGENT_MAXIMUM_ENERGY;
	extern int AGENT_STARTING_ENERGY;
	extern int AGENT_PASSIVE_COST;

	extern int ATTACK_COST;
	extern int COST_WHEN_ATTACKED;

	extern bool ALLOW_OVERCROUD_COST;
	extern int OVER_2_NEIGHBOURS_COST;

	extern bool USE_AUTOMATIC_SELECTION; // Experimental !!!!

}

class Simulation {
public:

	const int MapSize = 400;
	const int MapSizeHalf = MapSize / 2;

	std::vector<Agent*> Agents;
	std::mutex MutexRenderUpdateLoop;

	TileComponent** Map;

	void Init();
	void GenerateFoodMap(int FoodRarity, int MaxEnergy);
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