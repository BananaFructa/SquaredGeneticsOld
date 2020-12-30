#pragma once

#include <SFML/Graphics.hpp>
#include "NN/FFNN.h"

#define AGENT_INPUT_SIZE 305 // R(3x3) G(3x3) B(3x3) S(3x3) AR(3x3) AG(3x3) Energy(1) SignalIn(1) Memory(10) (RGBSA Sequence)
#define AGENT_OUTPUT_SIZE 17 // Left(1) Up(1) AttackH(1) AttackV(1) Signal (1) Eat (1) Reproduce(1) Memory(30)

#define HORIZONTAL_MOVMENT_NEURON 0
#define VERTICAL_MOVMENT_NEURON 1
#define ATTACK_HORIZONTAL 2
#define ATTACK_VERTICAL 3
#define SIGNAL_NEURON 4
#define EAT_NEURON 5
#define REPRODUCE_NEURON 6

#define AGENT_MEMORY_SIZE 10
#define AGENT_VIEW_AREA 3

class Agent {
public:
	sf::Vector2i Position;
	sf::RectangleShape Shape;
	sf::Color Color;

	FFNN* NNController;

	float Energy = 0;
	float Memory[AGENT_MEMORY_SIZE] = {0};
	bool CurrentSignalState = 0;
	bool Attacks = false;
	bool IsAttacked = false;
	bool AttackStateOfCurrentUpdate = false;
	int ReproductionCounts = 0;

	Agent(sf::Color Color, sf::Vector2i InitialPosition = sf::Vector2i(0, 0));
	Agent(Agent& agent);
	~Agent();
	void SetPosition(sf::Vector2i NewPositon);
	float* Update(float* InputData);
};