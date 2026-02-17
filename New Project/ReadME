# 🚀 Space Outage — Through the Unknown

## Software Architecture & Code Documentation

**Project Name:** Space Outage — Through the Unknown  
**Genre:** 2D Arcade / Action Shooter  
**Language:** C (C99)  
**Graphics & Audio Library:** Free assets from internet  
**Platform:** Windows

---

## 📑 Table of Contents

1. Executive Summary
2. Project Overview
3. Core Gameplay Systems
4. Project Structure
5. Entity Architecture
6. State Management
7. Memory Management Strategy
8. Physics & Math Systems
9. Rendering & Animation
10. Input Handling
11. Design Patterns Used
12. Known Limitations
13. Future Improvements
14. Conclusion

---

## 1. Executive Summary

**Space Outage — Through the Unknown** is a feature‑rich 2D arcade shooter developed in **C using Raylib**.  
The project demonstrates a transition from simple gameplay scripting to a **modular, system‑oriented architecture** focused on real‑time performance and maintainability.

Key strengths of the project include:
- Modular entity systems (Enemy, Alien, Bullet, Particle)
- Object pooling for performance stability
- Finite State Machine–based game flow
- Delta‑time driven physics
- Predictive enemy interception logic
- Multi‑phase gameplay (Spaceship → Ground combat)

---

## 2. Project Overview

### Gameplay Flow

1. **Spaceship Combat Phase**
   - Player controls a spaceship in free‑movement
   - Enemies spawn dynamically and intercept the player
   - Bullet‑based combat with cooldown
   - Particle booster effects for movement feedback

2. **Transition Phase**
   - Player spaceship is destroyed
   - Player character exits spaceship

3. **Ground Combat Phase**
   - Player controls an astronaut
   - Ground‑based alien enemies spawn periodically
   - Mouse‑aimed shooting with physics‑based bullets

---

## 3. Core Gameplay Systems

### Implemented Systems

- Player movement (spaceship & astronaut)
- Enemy AI with predictive targeting
- Bullet system with directional velocity
- Particle system (boosters & side boosters)
- Health & scoring system
- Music and sound effect management
- UI button system (Menu)

---

## 4. Project Structure

```
space-outage/
├── src/
│   ├── alien1.c        # Ground alien logic & animation
│   ├── booster.c       # Particle / booster system
│   ├── bullet.c        # Bullet physics & rendering
│   ├── enemy.c         # Aerial enemy AI & interception
│   ├── game_state.c    # Main loop & state control
│   └── ui.c            # Menu UI buttons
│
├── headers/
│   ├── alien1.h
│   ├── booster.h
│   ├── bullet.h
│   ├── enemy.h
│   ├── game_state.h
│   └── ui.h
│
├── assets/
│   ├── textures/
│   └── sounds/
│
└── README.md
```

Each module owns a **single responsibility** and exposes functionality through its header file.

---

## 5. Entity Architecture

### Alien (Ground Enemy)

**Files:** `alien1.c / alien1.h`

- Uses sprite‑sheet animation (8 frames)
- Time‑accumulator based animation (frame‑rate independent)
- Health‑based lifetime
- Horizontal movement toward player

Key Fields:
- `Rectangle rec` — world position & size
- `frame_timer` — animation timing
- `cur_frame` — sprite index
- `alive` — object pool state

---

### Enemy (Aerial Interceptor)

**Files:** `enemy.c / enemy.h`

- Predictive targeting toward player
- Locks target once within distance threshold
- Computes vertical velocity using interception math

Key Logic:
```
time_to_target = distance_x / velocity_x
y_velocity = vertical_distance / time_to_target
```

---

### Bullet System

**Files:** `bullet.c / bullet.h`

- Directional velocity vectors
- Delta‑time based movement
- Screen‑boundary deactivation
- Object pooling (fixed array)

No dynamic memory allocation during gameplay.

---

### Particle / Booster System

**Files:** `booster.c / booster.h`

- Continuous particle emission
- Alpha fading via life decay
- Radius shrink for visual realism
- Separate systems for main and side boosters

---

## 6. State Management

### Finite State Machine

```c
typedef enum {
    MENU,
    GAME,
    EXIT
} GameState;
```

- Menu state handles UI and reset logic
- Game state runs all simulation systems
- Exit state terminates application

This prevents gameplay logic from running outside its intended context.

---

## 7. Memory Management Strategy

### Object Pooling

All dynamic entities use **pre‑allocated arrays**:

- `Enemy enemies[10]`
- `Alien1 aliens1[20]`
- `bullet bullets[50]`
- `Particle boosters[100]`

Lifecycle:
1. Find inactive slot
2. Initialize data
3. Mark active
4. Deactivate when out of scope

This avoids:
- Heap fragmentation
- Frame drops from `malloc/free`

---

## 8. Physics & Math Systems

### Delta‑Time Movement

All movement uses:
```c
position += velocity * dt;
```

Ensures consistent gameplay across different FPS values.

### Gravity & Jump Physics

- Continuous gravity acceleration
- Velocity‑based jumping
- Ground collision correction

---

## 9. Rendering & Animation

- `DrawTexturePro()` used for scaling & rotation
- Sprite‑sheet animation via source rectangles
- Independent animation timers
- Layered rendering order (background → entities → UI)

---

## 10. Input Handling

- Keyboard: movement & jumping
- Mouse: shooting direction & UI buttons
- Input is processed per frame in GAME state only

---

## 11. Design Patterns Used

### Game Loop Pattern

```c
while (!WindowShouldClose()) {
    process_input();
    update_game();
    render_frame();
}
```

### Update Method Pattern

Each entity exposes:
- `update_*()`
- `draw_*()`

### Component‑Style Data Design

- Structs store data only
- Systems apply behavior

---

## 12. Known Limitations

- Large `main` function (needs further decomposition)
- Collision logic is brute‑force O(n²)
- No save/load system
- No resolution‑independent UI scaling

---

## 13. Future Improvements

- Separate input, physics, rendering systems
- Collision spatial partitioning (grid / quadtree)
- More enemy AI states
- Weapon variety
- Difficulty scaling system
- Codebase cleanup & refactor

---

## 14. Conclusion

**Space Outage — Through the Unknown** demonstrates:

- Strong understanding of C game architecture
- Performance‑oriented design decisions
- Practical use of mathematics in gameplay
- Modular and extensible code organization

This project serves as a **solid portfolio‑level demonstration** of real‑time software engineering using C and Raylib.

---

**End of Documentation**


