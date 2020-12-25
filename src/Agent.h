#pragma once

class Agent {
private:

	bool RequiresPositionUpdate = false;

public:
	sf::Vector2f Position;
	sf::RectangleShape Shape;
	sf::Color Color;

	Agent(sf::Color Color, sf::Vector2f InitialPosition = sf::Vector2f(0, 0));
	void SetPosition(sf::Vector2f NewPositon);
	void Update();
};