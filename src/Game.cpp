#include "Game.hpp"
#include "entities/Ball.hpp"
#include "entities/Obstacle.hpp"
#include <random>
#include <chrono>

Game::Game(unsigned int width, unsigned int height)
    : window(sf::VideoMode({width, height}), "Mini Golf", sf::Style::Default)
    , originalSize(static_cast<float>(width), static_cast<float>(height))
    , running(true)
    , tileSize(50.f)
    , obstacleGenerationDistance(500.f)
    , minObstacleDistance(100.f)
    , maxObstacleCount(30)
    , lastGenerationX(0.f)
    , lastGenerationY(0.f)
{
    // Initialize random number generator with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
    
    window.setFramerateLimit(144);
    
    // Initialize the game view
    gameView.setSize(originalSize);
    gameView.setCenter({originalSize.x / 2.f, originalSize.y / 2.f});
    window.setView(gameView);
    
    // Initialize tile shape
    tileShape.setSize({tileSize, tileSize});
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
    // Use a fixed 16:9 aspect ratio for the game content
    const float targetAspectRatio = 16.f / 9.f;
    
    // Calculate the new window aspect ratio
    float windowAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    // Set viewport to fill the entire window (no black bars)
    gameView.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
    
    // Use a scale factor to prevent objects from appearing too large in fullscreen
    // This acts like a zoom-out factor for larger screens
    float scaleFactorX = originalSize.x / static_cast<float>(width);
    float scaleFactorY = originalSize.y / static_cast<float>(height);
    float scaleFactor = std::min(scaleFactorX, scaleFactorY);
    
    // Apply a minimum zoom-out for larger screens
    // The smaller this number, the more "zoomed out" the view will be
    const float minScaleFactor = 0.4f;
    scaleFactor = std::max(scaleFactor, minScaleFactor);
    
    // Adjust the view size - larger viewWidth/viewHeight means we see more of the game world
    float viewWidth, viewHeight;
    
    if (windowAspectRatio > targetAspectRatio) {
        // Window is wider than 16:9
        viewHeight = originalSize.y / scaleFactor;
        viewWidth = viewHeight * windowAspectRatio;
    } else {
        // Window is taller than 16:9
        viewWidth = originalSize.x / scaleFactor;
        viewHeight = viewWidth / windowAspectRatio;
    }
    
    // Update the view size
    gameView.setSize({viewWidth, viewHeight});
    
    // Keep the ball centered if possible
    Ball* ball = findBall();
    if (ball) {
        gameView.setCenter(ball->getPosition());
    } else {
        gameView.setCenter({viewWidth / 2.f, viewHeight / 2.f});
    }
    
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
    
    // Center the view on the ball
    Ball* ball = findBall();
    if (ball) {
        // Get the ball's position
        sf::Vector2f ballPos = ball->getPosition();
        
        // Generate new obstacles if the ball moves far enough
        if (std::abs(ballPos.x - lastGenerationX) > obstacleGenerationDistance ||
            std::abs(ballPos.y - lastGenerationY) > obstacleGenerationDistance) {
            generateObstacles();
            lastGenerationX = ballPos.x;
            lastGenerationY = ballPos.y;
        }
        
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

void Game::generateObstacles() {
    Ball* ball = findBall();
    if (!ball) return;
    
    // Get current obstacle count
    auto obstacles = findObstacles();
    if (obstacles.size() >= maxObstacleCount) return;
    
    // Get the ball's position
    sf::Vector2f ballPos = ball->getPosition();
    
    // Define the generation area around the ball (focus on generating ahead)
    float genLeft = ballPos.x + 300.f;  // Generate ahead of the ball
    float genTop = ballPos.y - 600.f;
    float genWidth = 1200.f;
    float genHeight = 1200.f;
    
    // Random distributions
    std::uniform_real_distribution<float> xDist(genLeft, genLeft + genWidth);
    std::uniform_real_distribution<float> yDist(genTop, genTop + genHeight);
    std::uniform_real_distribution<float> sizeDist(40.f, 120.f);
    std::uniform_int_distribution<int> colorDist(0, 2);
    std::uniform_int_distribution<int> typeDist(0, 1);  // 0 for horizontal, 1 for vertical
    
    // Try to create 1-3 new obstacles
    std::uniform_int_distribution<int> countDist(1, 3);
    int newObstacleCount = countDist(rng);
    
    // Colors for obstacles
    sf::Color obstacleColors[] = {
        Colors::LightBrown,
        Colors::DarkBrown,
        Colors::Gray
    };
    
    for (int i = 0; i < newObstacleCount; i++) {
        // Generate random properties
        float x = xDist(rng);
        float y = yDist(rng);
        float width, height;
        
        if (typeDist(rng) == 0) { // Horizontal obstacle
            width = sizeDist(rng);
            height = sizeDist(rng) / 2.f;
        } else { // Vertical obstacle
            width = sizeDist(rng) / 2.f;
            height = sizeDist(rng);
        }
        
        sf::Vector2f position(x, y);
        sf::Vector2f size(width, height);
        
        // Only add if position is valid
        if (isValidObstaclePosition(position, size)) {
            sf::Color color = obstacleColors[colorDist(rng)];
            addEntity(std::make_unique<Obstacle>(position, size, color));
        }
    }
}

bool Game::isValidObstaclePosition(const sf::Vector2f& pos, const sf::Vector2f& size) {
    Ball* ball = findBall();
    if (!ball) return false;
    
    // Don't generate too close to the ball
    float distToBall = std::hypot(pos.x - ball->getPosition().x, 
                                 pos.y - ball->getPosition().y);
    if (distToBall < minObstacleDistance + ball->getRadius()) {
        return false;
    }
    
    // Check overlap with existing obstacles
    sf::FloatRect newObstacleBounds(
        {pos.x - size.x / 2.f, pos.y - size.y / 2.f}, 
        size
    );
    
    for (auto obstacle : findObstacles()) {
        sf::FloatRect existingBounds = obstacle->getBounds();
        
        // Add some padding to avoid obstacles being too close
        existingBounds.position.x -= 20.f;
        existingBounds.position.y -= 20.f;
        existingBounds.size.x += 40.f;
        existingBounds.size.y += 40.f;
        
        if (existingBounds.findIntersection(newObstacleBounds) != std::nullopt) {
            return false;
        }
    }
    
    return true;
} 