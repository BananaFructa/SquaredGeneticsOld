#include "Agent.h"

Agent::Agent(sf::Color Color, sf::Vector2i InitialPosition) {
	this->Color = Color;
	this->Position = InitialPosition;
	this->Shape = sf::RectangleShape(sf::Vector2f(0.9f, 0.9f));
	this->Shape.setPosition((sf::Vector2f)Position + sf::Vector2f(0.05f, 0.05f));
	this->Shape.setFillColor(this->Color);
	int NetworkArhitecture[] = { AGENT_INPUT_SIZE,50,50,AGENT_OUTPUT_SIZE };
	AFunction NetworkActivations[] = { AFunctions::LeakyReLU,AFunctions::LeakyReLU, AFunctions::Tanh };

	NNController = new FFNN(
		NetworkArhitecture,
		NetworkActivations,
		4
	);
}

Agent::Agent(Agent& agent) {
	this->Color = agent.Color;
	this->Position = agent.Position;
	this->Shape = sf::RectangleShape(sf::Vector2f(0.9f, 0.9f));
	this->Shape.setPosition((sf::Vector2f)Position + sf::Vector2f(0.05f, 0.05f));
	this->Shape.setFillColor(this->Color);
	this->NNController = new FFNN(*agent.NNController);
}

Agent::~Agent() {
	delete NNController;
}

void Agent::SetPosition(sf::Vector2i NewPositon) {
	this->Position = NewPositon;
	Shape.setPosition((sf::Vector2f)Position + sf::Vector2f(0.05f, 0.05f));
}

float* Agent::Update(float* Input) {
	return this->NNController->Predict(Input);
}