cmake_minimum_required(VERSION 3.16)
project(GeoWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Fetch raylib ─────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
    GIT_SHALLOW    TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    main.cpp

    # Infrastructure
    Infrastructure/ResourceManager.cpp

    # Core - Interfaces and State Management (header-only, no .cpp needed)

    # Model - Game Logic and Entities
    Models/Systems/GameModel.cpp
    Models/Systems/SessionStats.cpp
    Models/Systems/PhysicsSystem.cpp
    Models/Systems/CollisionSystem.cpp
    Models/Systems/EnemySpawner.cpp
    Models/Systems/AlienSpawner.cpp
    Models/Systems/BulletPool.cpp
    Models/Entities/Bullet.cpp
    Models/Entities/HomingEnemy.cpp
    Models/Entities/GroundAlien.cpp

    # Controllers - Input and Game Flow
    Controllers/GameController.cpp
    Controllers/InputHandler.cpp
    Controllers/SpaceshipController.cpp
    Controllers/AstronautController.cpp

    # Views - Rendering and UI
    Views/Rendering/GameRenderer.cpp
    Views/Rendering/ThrusterEffect.cpp
    Views/UI/HUD.cpp
    Views/UI/MenuView.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories for the modular structure
target_include_directories(${PROJECT_NAME} PRIVATE
    .
    Infrastructure
    Core/Interfaces
    Models/Systems
    Models/Entities
    Controllers
    Views/Rendering
    Views/UI
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# ── Windows: static link so the .exe is self-contained ───────────────────────
if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

# ── Copy assets next to the executable ───────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets to build directory"
)

# ── macOS / Linux linking ─────────────────────────────────────────────────────
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m dl pthread GL)
endif()
