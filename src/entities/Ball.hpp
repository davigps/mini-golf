#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "../utils/Entity.hpp"
#include <functional>

class Obstacle;

class Ball : public Entity {
public:
    // Callback types for events
    using CollisionCallback = std::function<void(const sf::Vector2f&, const sf::Vector2f&)>;
    using MovementCallback = std::function<void(const sf::Vector2f&, const sf::Vector2f&)>;
    
    Ball(float radius = 20.f);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    void drawShadow(sf::RenderWindow& window) override;
    bool handleMousePress(const sf::Vector2f& mousePos) override;
    bool handleMouseRelease(const sf::Vector2f& mousePos) override;
    bool handleMouseMove(const sf::Vector2f& mousePos) override;
    
    // Collision methods
    void checkCollision(const Obstacle& obstacle);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getVelocity() const { return velocity; }
    float getRadius() const { return shape.getRadius(); }
    
    // Set callbacks
    void setCollisionCallback(CollisionCallback callback) { onCollision = callback; }
    void setMovementCallback(MovementCallback callback) { onMovement = callback; }

private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f startDragPos;
    sf::Vector2f currentDragPos;
    sf::Vertex line[2];
    sf::Vertex arrowHead[3]; // Triangle for the arrowhead
    void updateArrowHead(); // Helper method to update the arrowhead
    bool isDragging;
    float friction;
    
    // Callbacks
    CollisionCallback onCollision;
    MovementCallback onMovement;
}; 