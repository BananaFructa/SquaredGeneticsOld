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
#define MAX_ENERGY_IN_GENERATED_TILE 1000

#define INITIAL_MUTATION_CHANCE 0.1f
#define CHANCE_OF_MUTATED_COPY 0.1f
#define MUTATION_CHANCE_ON_COPY 0.2f
#define MUTATION_APLITUDE_ON_COPY 0.12f

#define TILE_GROWTH 1LL

#define WALKING_COST 2
#define PASSIVE_COST 1
#define BIRTH_COST 30
#define AGENT_MINIMUM_ENERGY 0
#define AGENT_MAXIMUM_ENERGY 100
#define AGENT_STARTING_ENERGY 20

#define ENERGY_ABSORPTION_RATE 10

#define COLOR_CHANGE_VALUE 1
#define COLOR_CHANCE_CHANCE 0.1f

#define POPULATION_MINIMUM 60

class Simulation {
public:

	const int MapSize = 100;

	std::vector<Agent*> Agents;
	std::mutex MutexRenderUpdateLoop;

	// Byte structure
	//
	//    12 bits  12 bits  10 bits  2 bits  1 bit  8 bits  8 bits  8 bits
	//    |_____|  |_____|  |_____|  |____|  |___|  |____|  |____|  |____|
	//    MaxEngy  Energy   Growth   Attack  Signal    R      G       B
	//
	// 00 - No attack
	// 01 - Attacking
	// 10 - Being Attacked
	// 0 - No Signal
	// 1 - On Signal

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