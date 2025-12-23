cmake_minimum_required(VERSION 3.16)
project(RealisticRenderer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Platform Detection ---
if(APPLE)
    # macOS: OpenGL is deprecated but still works. Suppress warnings.
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

# --- Dependencies ---
# GLFW: Window and Input
find_package(glfw3 3.3 REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# --- Option: Use real PBR textures ---
option(USE_REAL_TEXTURES "Use downloaded PBR textures instead of procedural" OFF)
if(USE_REAL_TEXTURES)
    add_definitions(-DUSE_REAL_TEXTURES)
endif()

# --- Source Files ---
set(SOURCES
    src/main.cpp
    src/glad.c
    src/Shader.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/Texture.cpp
    src/Primitives.cpp
    src/Scene.cpp
    src/Level.cpp
    src/Ball.cpp
    src/BoardGenerator.cpp
    # ImGui
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_SOURCE_DIR}/external/glm
)

# --- Link Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)

# macOS specific frameworks
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# --- Symlink assets to build directory (always use source assets) ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
