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
    src/main.cpp

    # Core
    src/core/ResourceManager.cpp
    src/core/GameStateMachine.cpp

    # Model
    src/model/GameModel.cpp
    src/model/SessionStats.cpp
    src/model/entities/HomingEnemy.cpp
    src/model/entities/GroundAlien.cpp
    src/model/entities/Bullet.cpp
    src/model/entities/BulletPool.cpp
    src/model/systems/CollisionSystem.cpp
    src/model/systems/EnemySpawner.cpp
    src/model/systems/AlienSpawner.cpp
    src/model/systems/PhysicsSystem.cpp

    # View
    src/view/GameRenderer.cpp
    src/view/HUD.cpp
    src/view/MenuView.cpp
    src/view/ThrusterEffect.cpp

    # Controller
    src/controller/GameController.cpp
    src/controller/SpaceshipController.cpp
    src/controller/AstronautController.cpp
    src/controller/InputHandler.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

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
