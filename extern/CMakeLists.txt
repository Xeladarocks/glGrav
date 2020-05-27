
cmake_minimum_required (VERSION 3.8)

include(FindGit)
find_package(Git)
if (NOT Git_FOUND)
    message(FATAL_ERROR "Git not found!")
endif ()


include(FetchContent)

# -----= GLFW =----- #
FetchContent_Declare(
    GLFW                  # Recommendation: Stick close to the original name.
    GIT_REPOSITORY https://github.com/glfw/glfw/
    SOURCE_DIR     GLFW   # (Relative) path within in the build directory.
)
FetchContent_GetProperties(GLFW)
if(NOT GLFW_POPULATED)
    FetchContent_Populate(GLFW)
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
    include_directories(${glfw_SOURCE_DIR}/include)
endif()
# -----=      =----- #

# -----= GLAD =----- #
FetchContent_Declare(
    glad                  # Recommendation: Stick close to the original name.
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    SOURCE_DIR glad
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
    include_directories(${glad_SOURCE_DIR}/include)
endif()
# -----=      =----- #

# -----= GLM =----- #
FetchContent_Declare(
    glm                  # Recommendation: Stick close to the original name.
    GIT_REPOSITORY https://github.com/g-truc/glm
    SOURCE_DIR glm
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
    include_directories(${glm_SOURCE_DIR})
endif()
# -----=     =----- #