#pragma once

#include <SFML/Graphics.hpp>
#include "../utils/Entity.hpp"
#include "../utils/Colors.hpp"

class Obstacle : public Entity {
public:
    Obstacle(const sf::Vector2f& position, const sf::Vector2f& size, 
             const sf::Color& color = Colors::Gray);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    void drawShadow(sf::RenderWindow& window) override;
    
    // Standard bounds calculation (for non-collision uses)
    sf::FloatRect getBounds() const;
    
    // Set the rotation of the obstacle in degrees
    void setRotation(float angle);
    
    // Get the center position of the obstacle
    sf::Vector2f getPosition() const;
    
    // Get the rotation angle in degrees
    float getRotation() const;
    
    // Get the size of the obstacle
    sf::Vector2f getSize() const;
    
    // Check precise collision with a circle (for ball collision)
    bool checkCircleCollision(const sf::Vector2f& circleCenter, float radius, 
                             sf::Vector2f& collisionPoint, sf::Vector2f& collisionNormal) const;

private:
    // Calculate the four corners of the rotated rectangle
    std::array<sf::Vector2f, 4> getCorners() const;
    
    // Distance from point to line segment
    float distancePointLineSegment(const sf::Vector2f& point, 
                                  const sf::Vector2f& lineStart, 
                                  const sf::Vector2f& lineEnd,
                                  sf::Vector2f& closestPoint) const;
    
    sf::RectangleShape shape;
}; 