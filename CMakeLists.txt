cmake_minimum_required(VERSION 3.10)

# Set the project name
project(test)

# Find the SFML package
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME} 
src/main.cpp
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)