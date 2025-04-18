#include "Game.hpp"
#include "entities/Ball.hpp"
#include "entities/Obstacle.hpp"

Game::Game(unsigned int width, unsigned int height)
    : window(sf::VideoMode({width, height}), "Mini Golf", sf::Style::Default)
    , originalSize(static_cast<float>(width), static_cast<float>(height))
    , running(true)
{
    window.setFramerateLimit(144);
    
    // Initialize the game view
    gameView.setSize(originalSize);
    gameView.setCenter({originalSize.x / 2.f, originalSize.y / 2.f});
    window.setView(gameView);
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
        
        if (const auto* windowResized = event->getIf<sf::Event::Resized>()) {
            handleResize(windowResized->size.x, windowResized->size.y);
        }
        
        if (event->is<sf::Event::MouseButtonPressed>() && 
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMousePress(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        if (event->is<sf::Event::MouseButtonReleased>()) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMouseRelease(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
        
        if (event->is<sf::Event::MouseMoved>()) {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = mapPixelToCoords(mousePixelPos);
            
            for (auto& entity : entities) {
                if (entity->handleMouseMove(mousePos)) {
                    break; // Break after first entity handles the event
                }
            }
        }
    }
}

void Game::handleResize(unsigned int width, unsigned int height) {
    // Calculate the aspect ratio of the original window
    float originalAspectRatio = originalSize.x / originalSize.y;
    
    // Calculate the new aspect ratio
    float newAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // Set the viewport to maintain aspect ratio
    float viewportWidth = 1.f;
    float viewportHeight = 1.f;
    float viewportLeft = 0.f;
    float viewportTop = 0.f;
    
    if (newAspectRatio > originalAspectRatio) {
        // Window is wider than it should be - add black bars on the sides
        viewportWidth = originalAspectRatio / newAspectRatio;
        viewportLeft = (1.f - viewportWidth) / 2.f;
    } else if (newAspectRatio < originalAspectRatio) {
        // Window is taller than it should be - add black bars on top/bottom
        viewportHeight = newAspectRatio / originalAspectRatio;
        viewportTop = (1.f - viewportHeight) / 2.f;
    }
    
    // Apply the new viewport
    gameView.setViewport(sf::FloatRect({viewportLeft, viewportTop}, {viewportWidth, viewportHeight}));
    
    // Keep the original size for the view
    gameView.setSize(originalSize);
    gameView.setCenter({originalSize.x / 2.f, originalSize.y / 2.f});
    
    window.setView(gameView);
}

sf::Vector2f Game::mapPixelToCoords(const sf::Vector2i& pixelPos) const {
    return window.mapPixelToCoords(pixelPos, gameView);
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