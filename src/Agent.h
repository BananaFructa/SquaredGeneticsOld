#pragma once

#include <SFML/Graphics.hpp>
#include "NN/FFNN.h"

#define AGENT_INPUT_SIZE 519 // R(9x9) G(9x9) B(9x9) S(9x9) AR(9x9) AG(9x9) SelfEnergy(1) Energy(1) SignalIn(1) Memory(30) (RGBSA Sequence)
#define AGENT_OUTPUT_SIZE 39 // Left(1) Right(1) Up(1) Down(1) Attack(1) AttackAngle (1) Signal (1) Eat (1) Reproduce(1) Memory(30)

#define LEFT_NEURON 0
#define RIGHT_NEURON 1
#define UP_NEURON 2
#define DOWN_NEURON 3
#define ATTACK_NEURON 4
#define ATTACK_ANGLE_NEURON 5
#define SIGNAL_NEURON 6
#define EAT_NEURON 7
#define REPRODUCE_NEURON 8

#define AGENT_MEMORY_SIZE 30
#define AGENT_VIEW_AREA 81

class Agent {
public:
	sf::Vector2i Position;
	sf::RectangleShape Shape;
	sf::Color Color;

	FFNN* NNController;

	float Energy;
	float Memory[AGENT_MEMORY_SIZE];
	bool CurrentSignalState;
	bool Attacks;
	bool IsGettingAttacked;

	Agent(sf::Color Color, sf::Vector2i InitialPosition = sf::Vector2i(0, 0));
	void SetPosition(sf::Vector2i NewPositon);
	float* Update(float* InputData);
};