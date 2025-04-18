#include "Game.hpp"
#include "../entities/Ball.hpp"
#include "../entities/Obstacle.hpp"
#include "../systems/PhysicsSystem.hpp"
#include "../systems/InputHandler.hpp"
#include "../systems/ObstacleGenerator.hpp"
#include <random>
#include <chrono>

Game::Game(unsigned int width, unsigned int height)
    : window(sf::VideoMode({width, height}), "Mini Golf", sf::Style::Default)
    , originalSize(static_cast<float>(width), static_cast<float>(height))
    , running(true)
    , tileSize(50.f)
{
    window.setFramerateLimit(144);
    
    // Initialize the game view with 16:9 aspect ratio
    // We'll base it on the original height to keep the same vertical size
    float height16by9 = originalSize.y;
    float width16by9 = height16by9 * (16.f / 9.f);
    
    // Store the 16:9 view size for later use
    gameViewSize = sf::Vector2f(width16by9, height16by9);
    
    // Initialize the game view with 16:9 aspect ratio
    gameView.setSize(gameViewSize);
    gameView.setCenter({gameViewSize.x / 2.f, gameViewSize.y / 2.f});
    window.setView(gameView);
    
    // Initialize tile shape
    tileShape.setSize({tileSize, tileSize});
    
    // Initialize systems
    physicsSystem = std::make_unique<PhysicsSystem>();
    inputHandler = std::make_unique<InputHandler>(window);
    obstacleGenerator = std::make_unique<ObstacleGenerator>();
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
    
    // Then draw all entities
    for (auto& entity : entities) {
        entity->draw(window);
    }
    
    window.display();
}

void Game::handleResize(unsigned int width, unsigned int height) {
    // Update window dimensions without changing view yet
    window.setSize({width, height});
    
    // Store current center
    sf::Vector2f currentCenter = gameView.getCenter();
    
    // Always use 16:9 aspect ratio for the game view
    // This ensures the view maintains a consistent aspect ratio
    gameView.setSize(gameViewSize);
    
    // Calculate viewport to fill the window
    float viewportWidth = 1.0f;
    float viewportHeight = 1.0f;
    float viewportLeft = 0.0f;
    float viewportTop = 0.0f;
    
    // Calculate window aspect ratio
    float windowAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float gameAspectRatio = 16.f / 9.f; // Fixed 16:9 ratio
    
    if (windowAspectRatio > gameAspectRatio) {
        // Window is wider than 16:9, we'll fill the height and see more horizontally
        viewportHeight = 1.0f;
        viewportWidth = 1.0f;
        viewportLeft = 0.0f;
        viewportTop = 0.0f;
    } else {
        // Window is taller than 16:9, we'll fill the width and see more vertically
        viewportWidth = 1.0f;
        viewportHeight = 1.0f;
        viewportLeft = 0.0f;
        viewportTop = 0.0f;
    }
    
    // Set the viewport to fill the window
    gameView.setViewport(sf::FloatRect({viewportLeft, viewportTop}, {viewportWidth, viewportHeight}));
    
    // Maintain current center (follow ball)
    gameView.setCenter(currentCenter);
    
    // Apply the view to the window
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