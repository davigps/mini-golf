#include "Game.hpp"
#include "entities/Ball.hpp"
#include "entities/Obstacle.hpp"

Game::Game(unsigned int width, unsigned int height)
    : window(sf::VideoMode({width, height}), "Mini Golf")
    , running(true)
{
    window.setFramerateLimit(144);
}

void Game::run() {
    while (running && window.isOpen()) {
        processEvents();
        
        auto deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        render();
    }
}

void Game::addEntity(std::unique_ptr<Entity> entity) {
    entities.push_back(std::move(entity));
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            running = false;
        }
        
        if (event->is<sf::Event::MouseButtonPressed>() && 
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2f mousePos(
                static_cast<float>(sf::Mouse::getPosition(window).x),
                static_cast<float>(sf::Mouse::getPosition(window).y)
            );
            
            for (auto& entity : entities) {
                if (entity->handleMousePress(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        if (event->is<sf::Event::MouseButtonReleased>()) {
            sf::Vector2f mousePos(
                static_cast<float>(sf::Mouse::getPosition(window).x),
                static_cast<float>(sf::Mouse::getPosition(window).y)
            );
            
            for (auto& entity : entities) {
                if (entity->handleMouseRelease(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        if (event->is<sf::Event::MouseMoved>()) {
            sf::Vector2f mousePos(
                static_cast<float>(sf::Mouse::getPosition(window).x),
                static_cast<float>(sf::Mouse::getPosition(window).y)
            );
            
            for (auto& entity : entities) {
                if (entity->handleMouseMove(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    for (auto& entity : entities) {
        entity->update(deltaTime);
    }
    
    // Check for collisions after updating positions
    checkCollisions();
}

void Game::render() {
    window.clear();
    
    for (auto& entity : entities) {
        entity->draw(window);
    }
    
    window.display();
}

void Game::checkCollisions() {
    Ball* ball = findBall();
    if (!ball) return;
    
    // Check collisions with all obstacles
    std::vector<Obstacle*> obstacles = findObstacles();
    for (auto obstacle : obstacles) {
        ball->checkCollision(*obstacle);
    }
}

Ball* Game::findBall() {
    for (auto& entity : entities) {
        Ball* ball = dynamic_cast<Ball*>(entity.get());
        if (ball) return ball;
    }
    return nullptr;
}

std::vector<Obstacle*> Game::findObstacles() {
    std::vector<Obstacle*> obstacles;
    for (auto& entity : entities) {
        Obstacle* obstacle = dynamic_cast<Obstacle*>(entity.get());
        if (obstacle) obstacles.push_back(obstacle);
    }
    return obstacles;
} 