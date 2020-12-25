#pragma once

class Simulation {
public:

	const int MapSize = 1000;

	std::vector<Agent*> Agents;

	uint64_t Map[MapSize][MapSize];

	void Init();
	void UpdateSimulation();
	void AddAgent(sf::Color Color, sf::Vector2f Position);
};