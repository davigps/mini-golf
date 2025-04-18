# Mini Golf

A simple 2D mini golf game built with C++ and SFML. This project allows players to aim and shoot a ball through a course with various obstacles.

## Features

- Physics-based ball movement with friction
- Click and drag to aim and shoot the ball
- Collision detection with obstacles
- Simple course layout with walls and obstacles

## Building the Project

1. Install [Git](https://git-scm.com/downloads) and [CMake](https://cmake.org/download/). Use your system's package manager if available.

2. Clone the repository and navigate to the project directory.

3. If you use Linux, install SFML's dependencies using your system package manager. On Ubuntu and other Debian-based distributions you can use the following commands:
   ```
   sudo apt update
   sudo apt install \
       libxrandr-dev \
       libxcursor-dev \
       libxi-dev \
       libudev-dev \
       libfreetype-dev \
       libflac-dev \
       libvorbis-dev \
       libgl1-mesa-dev \
       libegl1-mesa-dev \
       libfreetype-dev
   ```

4. Configure and build your project using CMake:
   ```
   cmake -B build
   cmake --build build
   ```

5. Run the game:
   ```
   ./build/mini-golf
   ```

## How to Play

- Left-click and drag from the ball to set direction and power
- Release to shoot the ball
- Try to navigate through the course by avoiding obstacles

## Dependencies

This project uses:
- [SFML 3.0.0](https://www.sfml-dev.org/) for graphics and input handling
- CMake for build system

## License

The source code is dual licensed under Public Domain and MIT -- choose whichever you prefer.
