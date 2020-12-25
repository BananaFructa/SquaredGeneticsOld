#pragma once

void NormalizeViewport(float fov,sf::Vector2f CameraPosition) {
    sf::Vector2f WindowResolution = sf::Vector2f((float)Window.getSize().x, (float)Window.getSize().y);
    sf::Vector2f ViewProportion = sf::Vector2f(WindowResolution.x / fov, WindowResolution.y / fov);
    sf::View MainView = sf::View(CameraPosition, ViewProportion);
    Window.setView(MainView);
}

void DrawLine(float x1,float y1,float x2,float y2,sf::Color Color) {
    sf::VertexArray lines(sf::LinesStrip, 2);
    lines[0].position = sf::Vector2f(x1, y1);
    lines[1].position = sf::Vector2f(x2, y2);
    lines[0].color = Color;
    lines[1].color = Color;
    Window.draw(lines);
}

void DrawStripedSquare(float x, float y, sf::Color Color) {
    x -= 0.05f;
    y -= 0.05f;
    sf::VertexArray lines(sf::LinesStrip, 2);
    lines[0].position = sf::Vector2f(x, y);
    lines[1].position = sf::Vector2f(x+0.95, y+0.2);
    lines[0].color = Color;
    lines[1].color = Color;
    Window.draw(lines);
}

int XUnitsCount(float fov) {
    return (int)((float)Window.getSize().x / fov);
}

int YUnitsCount(float fov) {
    return (int)((float)Window.getSize().y / fov);
}

float Fract(float f) {
    return f - (int)f;
}

void DrawGrid(float fov) {
    int XUnits = XUnitsCount(fov);
    int YUnits = YUnitsCount(fov);

    // It's floored so it creates the moving effect
    float XF = (int)CameraPosition.x;
    float YF = (int)CameraPosition.y;

    for (int i = -XUnits - 1;i <= XUnits + 1;i++) {
        DrawLine(i + XF, -100 + YF, i + XF, 100 + YF, sf::Color::Color(35, 35, 35));
    }

    for (int j = -YUnits - 1;j <= YUnits + 1;j++) {
        DrawLine(-100 + XF, j + YF, 100 + XF, j + YF, sf::Color::Color(35, 35, 35));
    }
}