
cmake_minimum_required (VERSION 3.8)

add_executable (${PROJECT_NAME} main.cpp Shader.cpp)

target_link_libraries(${PROJECT_NAME} glfw glad glm)