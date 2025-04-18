#include "Game.hpp"
#include "../entities/Ball.hpp"
#include "../entities/Obstacle.hpp"
#include "../systems/PhysicsSystem.hpp"
#include "../systems/InputHandler.hpp"
#include "../systems/ObstacleGenerator.hpp"
#include "../systems/ParticleSystem.hpp"
#include <random>
#include <chrono>
#include <vector>
#include <cmath>
#include <utility>
#include <map>

// Define standard aspect ratios
const std::vector<std::pair<std::string, float>> STANDARD_RATIOS = {
    {"4:3", 4.0f/3.0f},
    {"16:9", 16.0f/9.0f},
    {"16:10", 16.0f/10.0f},
    {"21:9", 21.0f/9.0f},
    {"32:9", 32.0f/9.0f}
};

// Define zoom factors for each aspect ratio (smaller value = more zoomed out)
const std::map<std::string, float> ZOOM_FACTORS = {
    {"4:3", 1.0f},
    {"16:9", 0.8f},  // More zoomed out for 16:9
    {"16:10", 1.0f},
    {"21:9", 1.0f},
    {"32:9", 1.0f}
};

Game::Game(unsigned int width, unsigned int height)
    : window(sf::VideoMode({width, height}), "Mini Golf", sf::Style::Default)
    , originalSize(static_cast<float>(width), static_cast<float>(height))
    , running(true)
    , tileSize(50.f)
{
    window.setFramerateLimit(144);
    
    // Calculate window's aspect ratio
    float windowAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // Find closest standard aspect ratio
    float closestRatio = findClosestAspectRatio(windowAspectRatio);
    std::string ratioName = "";
    
    // Find ratio name for zoom factor
    for (const auto& [name, ratio] : STANDARD_RATIOS) {
        if (std::abs(ratio - closestRatio) < 0.01f) {
            ratioName = name;
            break;
        }
    }
    
    // Get zoom factor (default to 1.0 if not found)
    float zoomFactor = 1.0f;
    if (!ratioName.empty() && ZOOM_FACTORS.find(ratioName) != ZOOM_FACTORS.end()) {
        zoomFactor = ZOOM_FACTORS.at(ratioName);
    }
    
    // Base the view size on the original height to keep vertical size consistent
    float viewHeight = originalSize.y / zoomFactor;
    float viewWidth = viewHeight * closestRatio;
    
    // Store the view size for later use
    gameViewSize = sf::Vector2f(viewWidth, viewHeight);
    
    // Initialize the game view with selected aspect ratio
    gameView.setSize(gameViewSize);
    gameView.setCenter({gameViewSize.x / 2.f, gameViewSize.y / 2.f});
    window.setView(gameView);
    
    // Initialize tile shape
    tileShape.setSize({tileSize, tileSize});
    
    // Initialize systems
    physicsSystem = std::make_unique<PhysicsSystem>();
    inputHandler = std::make_unique<InputHandler>(window);
    obstacleGenerator = std::make_unique<ObstacleGenerator>();
    particleSystem = std::make_unique<ParticleSystem>();
}

float Game::findClosestAspectRatio(float targetRatio) {
    float closestDiff = std::numeric_limits<float>::max();
    float closestRatio = 16.0f / 9.0f; // Default to 16:9 if no closer match
    
    for (const auto& [name, ratio] : STANDARD_RATIOS) {
        float diff = std::abs(ratio - targetRatio);
        if (diff < closestDiff) {
            closestDiff = diff;
            closestRatio = ratio;
        }
    }
    
    return closestRatio;
}

Game::~Game() {
    // Cleanup
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
    // Special handling for Ball to set up collision callback
    if (auto ball = dynamic_cast<Ball*>(entity.get())) {
        ball->setCollisionCallback([this](const sf::Vector2f& position, const sf::Vector2f& normal) {
            // Create particles at collision point
            particleSystem->createCollisionParticles(position, normal);
        });
        
        // Set up movement callback for the ball
        ball->setMovementCallback([this](const sf::Vector2f& position, const sf::Vector2f& direction) {
            // Create green particles when the ball starts moving
            particleSystem->createMovementParticles(position, direction);
        });
    }
    
    entities.push_back(std::move(entity));
}

void Game::processEvents() {
    // Use our new InputHandler to process events
    running = inputHandler->processEvents(entities);
    
    // Handle window resize (still needs to be in Game for now)
    while (std::optional<sf::Event> event = window.pollEvent()) {
        if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            handleResize(resized->size.x, resized->size.y);
        }
    }
}

void Game::update(float deltaTime) {
    // Use the physics system for entity updates and collisions
    physicsSystem->update(entities, deltaTime);
    
    // Update particle system
    particleSystem->update(deltaTime);
    
    // Get the ball for obstacle generation and view centering
    Ball* ball = findBall();
    if (ball) {
        // Get the ball's position
        sf::Vector2f ballPos = ball->getPosition();
        
        // Generate new obstacles if needed
        if (obstacleGenerator->shouldGenerateObstacles(ballPos)) {
            auto obstacles = findObstacles();
            obstacleGenerator->generateObstacles(ballPos, entities, obstacles);
        }
        
        // Check for collisions
        physicsSystem->checkCollisions(ball, findObstacles());
        
        // Center the view on the ball
        gameView.setCenter(ballPos);
        window.setView(gameView);
    }
}

void Game::render() {
    window.clear(); // Still clear the window to handle areas outside the view
    
    // Set view for drawing
    window.setView(gameView);
    
    // Draw the tiled background
    drawBackground();
    
    // First draw all shadows
    for (auto& entity : entities) {
        entity->drawShadow(window);
    }
    
    // Draw particles (between shadows and entities)
    particleSystem->draw(window);
    
    // Then draw all entities
    for (auto& entity : entities) {
        entity->draw(window);
    }
    
    window.display();
}

void Game::handleResize(unsigned int width, unsigned int height) {
    // Update window dimensions
    window.setSize({width, height});
    
    // Store current center position
    sf::Vector2f currentCenter = gameView.getCenter();
    
    // Calculate window's aspect ratio
    float windowAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // Find closest standard aspect ratio
    float closestRatio = findClosestAspectRatio(windowAspectRatio);
    
    // Find ratio name for zoom factor
    std::string ratioName = "";
    for (const auto& [name, ratio] : STANDARD_RATIOS) {
        if (std::abs(ratio - closestRatio) < 0.01f) {
            ratioName = name;
            break;
        }
    }
    
    // Get zoom factor (default to 1.0 if not found)
    float zoomFactor = 1.0f;
    if (!ratioName.empty() && ZOOM_FACTORS.find(ratioName) != ZOOM_FACTORS.end()) {
        zoomFactor = ZOOM_FACTORS.at(ratioName);
    }
    
    // Adapt view to match the selected aspect ratio while keeping objects at fixed size
    // Base the view size on the original height to keep vertical size consistent
    float viewHeight = originalSize.y / zoomFactor;
    float viewWidth = viewHeight * closestRatio;
    
    // Update stored view size
    gameViewSize = sf::Vector2f(viewWidth, viewHeight);
    
    // Set new view size to match selected aspect ratio
    gameView.setSize(gameViewSize);
    
    // Set the viewport to fill the entire window
    gameView.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
    
    // Maintain the center position (usually following the ball)
    gameView.setCenter(currentCenter);
    
    // Apply updated view to the window
    window.setView(gameView);
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

void Game::drawBackground() {
    // Get the view bounds
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    
    // Calculate the visible area
    float left = viewCenter.x - viewSize.x / 2.f;
    float top = viewCenter.y - viewSize.y / 2.f;
    float right = viewCenter.x + viewSize.x / 2.f;
    float bottom = viewCenter.y + viewSize.y / 2.f;
    
    // Calculate the start and end tile indices
    // Add extra 5 tiles in each direction for smoother scrolling
    int startRow = static_cast<int>(top / tileSize) - 5;
    int endRow = static_cast<int>(bottom / tileSize) + 5;
    int startCol = static_cast<int>(left / tileSize) - 5;
    int endCol = static_cast<int>(right / tileSize) + 5;
    
    // Draw the tiles
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            // Alternate tile colors in a checkerboard pattern
            bool isEvenTile = (row + col) % 2 == 0;
            tileShape.setFillColor(isEvenTile ? Colors::LightGreen : Colors::DarkGreen);
            
            // Position the tile
            tileShape.setPosition(sf::Vector2f(col * tileSize, row * tileSize));
            
            // Draw the tile
            window.draw(tileShape);
        }
    }
} 