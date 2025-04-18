#include "Ball.hpp"
#include "Obstacle.hpp"
#include "../Colors.hpp"
#include <cmath>

Ball::Ball(float radius) 
    : position(300.f, 300.f)
    , velocity(0.f, 0.f)
    , isDragging(false)
    , friction(0.99f)
{
    shape.setRadius(radius);
    shape.setFillColor(Colors::BallColor);
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setPosition(position);
    
    // Initialize line for drag visualization
    line[0] = {position, Colors::DragLineColor};
    line[1] = {position, Colors::DragLineColor};
}

void Ball::update(float deltaTime) {
    if (!isDragging) {
        // Apply friction to slow down the ball
        velocity *= friction;
        
        // Stop the ball if it's moving very slowly
        if (std::abs(velocity.x) < 1.0f && std::abs(velocity.y) < 1.0f) {
            velocity = sf::Vector2f(0.f, 0.f);
        }
        
        // Update position based on velocity
        position += velocity * deltaTime;
    }
    
    // Update the ball's shape position
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

bool Ball::handleMousePress(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isDragging = true;
        startDragPos = position; // Start from ball position, not mouse position
        currentDragPos = mousePos;
        velocity = sf::Vector2f(0.f, 0.f);
        return true;
    }
    return false;
}

bool Ball::handleMouseRelease(const sf::Vector2f& mousePos) {
    if (isDragging) {
        isDragging = false;
        // Calculate velocity based on drag distance and direction
        // The direction is reversed (position - mousePos instead of mousePos - position)
        // Intensity is proportional to the distance
        sf::Vector2f dragVector = position - mousePos;
        float distance = std::sqrt(dragVector.x * dragVector.x + dragVector.y * dragVector.y);
        
        // Apply a scaling factor to convert distance to appropriate velocity
        float factor = 2.5f;
        velocity = dragVector * factor;
        return true;
    }
    return false;
}

bool Ball::handleMouseMove(const sf::Vector2f& mousePos) {
    if (isDragging) {
        currentDragPos = mousePos;
        
        // Update the line end point to show drag direction and strength
        line[1].position = mousePos;
        return true;
    }
    return false;
}

sf::FloatRect Ball::getBounds() const {
    return shape.getGlobalBounds();
}

void Ball::checkCollision(const Obstacle& obstacle) {
    // Skip collision check if not moving
    if (velocity.x == 0 && velocity.y == 0) {
        return;
    }
    
    // Get obstacle bounds
    sf::FloatRect obstacleBounds = obstacle.getBounds();
    
    // Calculate the center point of the ball
    float ballRadius = shape.getRadius();
    sf::Vector2f ballCenter = position;
    
    // Find the closest point on the obstacle to the ball center
    sf::Vector2f closestPoint;
    
    // Clamp ball center to obstacle bounds for closest point
    auto obstacleWidth = obstacleBounds.size.x;
    auto obstacleLeft = obstacleBounds.position.x;
    auto obstacleHeight = obstacleBounds.size.y;
    auto obstacleTop = obstacleBounds.position.y;

    closestPoint.x = std::max(obstacleLeft, std::min(ballCenter.x, obstacleLeft + obstacleWidth));
    closestPoint.y = std::max(obstacleTop, std::min(ballCenter.y, obstacleTop + obstacleHeight));
    
    // Calculate the distance between the ball center and closest point
    sf::Vector2f ballToClosest = ballCenter - closestPoint;
    float distanceSquared = ballToClosest.x * ballToClosest.x + ballToClosest.y * ballToClosest.y;
    
    // Check if collision occurred (distance is less than ball radius)
    if (distanceSquared <= ballRadius * ballRadius) {
        // Normalize the direction vector
        float distance = std::sqrt(distanceSquared);
        sf::Vector2f direction = distance > 0 ? ballToClosest / distance : sf::Vector2f(0, -1);
        
        // Move the ball outside the obstacle along collision normal
        float overlap = ballRadius - distance;
        position += direction * overlap;
        
        // Reflect velocity based on collision normal (with some energy loss)
        float speedLoss = 0.8f; // Ball loses some energy on collision
        
        // Calculate dot product of velocity and normal
        float dotProduct = velocity.x * direction.x + velocity.y * direction.y;
        
        // Apply reflection formula: v' = v - 2(v·n)n
        velocity -= 2.0f * dotProduct * direction * speedLoss;
        
        // Update ball position
        shape.setPosition(position);
        line[0].position = position;
    }
} 