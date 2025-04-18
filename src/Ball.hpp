#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Ball {
public:
    Ball(float radius = 20.f);
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void handleMousePress(const sf::Vector2f& mousePos);
    void handleMouseRelease(const sf::Vector2f& mousePos);
    void handleMouseMove(const sf::Vector2f& mousePos);

private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f startDragPos;
    sf::Vector2f currentDragPos;
    sf::Vertex line[2];
    bool isDragging;
    float friction;
}; 