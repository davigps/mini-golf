#include "Ball.hpp"

Ball::Ball(float radius) 
    : position(300.f, 300.f)
    , velocity(0.f, 0.f)
    , isDragging(false)
    , friction(0.99f)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setPosition(position);
    
    // Initialize line for drag visualization
    line[0] = {position, sf::Color::Red, position};
    line[1] = {position, sf::Color::Red, position};
}

void Ball::update(float deltaTime) {
    if (!isDragging) {
        velocity *= friction;
        position += velocity * deltaTime;
        
        // Keep ball within window bounds
        float radius = shape.getRadius();
        if (position.x < radius) {
            position.x = radius;
            velocity.x *= -0.5f;
        }
        if (position.x > 600 - radius) {
            position.x = 600 - radius;
            velocity.x *= -0.5f;
        }
        if (position.y < radius) {
            position.y = radius;
            velocity.y *= -0.5f;
        }
        if (position.y > 600 - radius) {
            position.y = 600 - radius;
            velocity.y *= -0.5f;
        }
    }
    
    shape.setPosition(position);
    // Update line start position to follow ball
    line[0].position = position;
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(shape);
    
    // Draw the drag line when dragging
    if (isDragging) {
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void Ball::handleMousePress(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isDragging = true;
        startDragPos = position; // Start from ball position, not mouse position
        currentDragPos = mousePos;
        velocity = sf::Vector2f(0.f, 0.f);
    }
}

void Ball::handleMouseRelease(const sf::Vector2f& mousePos) {
    if (isDragging) {
        isDragging = false;
        // Calculate velocity based on drag distance and direction
        // The direction is reversed (position - mousePos instead of mousePos - position)
        // Intensity is proportional to the distance
        sf::Vector2f dragVector = position - mousePos;
        float distance = sqrt(dragVector.x * dragVector.x + dragVector.y * dragVector.y);
        
        // Apply a scaling factor to convert distance to appropriate velocity
        float factor = 2.5f;
        velocity = dragVector * factor;
    }
}

void Ball::handleMouseMove(const sf::Vector2f& mousePos) {
    if (isDragging) {
        currentDragPos = mousePos;
        
        // Update the line end point to show drag direction and strength
        line[1].position = mousePos;
    }
} 