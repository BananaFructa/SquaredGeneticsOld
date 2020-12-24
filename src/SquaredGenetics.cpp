#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

const sf::Vector2f DefaultSize = sf::Vector2f(100, 100);

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::RectangleShape shape(sf::Vector2f(0.5, 0.5));
    shape.setFillColor(sf::Color::Green);

    sf::View MainView;

    while (window.isOpen())
    {
        sf::Event event;

        sf::Vector2f WindowResolution = sf::Vector2f((float)window.getSize().x,(float)window.getSize().y);

        sf::Vector2f ViewProportion = sf::Vector2f(WindowResolution.x / DefaultSize.x, WindowResolution.y / DefaultSize.y);

        MainView = sf::View(sf::Vector2f(0, 0), ViewProportion);

        window.setView(MainView);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
