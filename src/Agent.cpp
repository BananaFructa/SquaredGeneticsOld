#include <SFML/Graphics.hpp>
#include "Agent.h"

Agent::Agent(sf::Color Color, sf::Vector2f InitialPosition) {
	this->Color = Color;
	this->Position = InitialPosition;
	this->Shape = sf::RectangleShape(sf::Vector2f(0.9f, 0.9f));
	this->Shape.setPosition(Position + sf::Vector2f(0.05f, 0.05f));
	this->Shape.setFillColor(this->Color);
}

void Agent::SetPosition(sf::Vector2f NewPositon) {
	this->Position = NewPositon;
	RequiresPositionUpdate = true;
}

void Agent::Update() {
	if (RequiresPositionUpdate) {
		Shape.setPosition(Position);
		RequiresPositionUpdate = false;
	}
}