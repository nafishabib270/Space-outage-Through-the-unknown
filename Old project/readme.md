# Space Outage: Through The Unknown

## 2D Endless Runner Game Documentation

---

## Project Title
**Space Outage: Through The Unknown**

---

## Description

**Space Outage** is a fast-paced 2D endless runner game developed in **C** using the **Raylib** graphics library. The game opens with a short animated cinematic where an astronaut crash-lands on a mysterious alien planet after a spacecraft malfunction. Shortly after landing, hostile alien creatures begin attacking, leaving the astronaut with only one option: **run and survive**.

Players control the astronaut in a side-scrolling environment filled with dynamic hazards and fast-moving enemies. The gameplay emphasizes fluid movement and responsive controls, allowing the player to **jump, slide, dodge, and shoot in any direction using mouse-based aiming**.

The game features a **coin collection system**, where gathering enough coins fills a meter that temporarily opens portals to new worlds with unique visuals and challenges. A **life bar system** tracks player health—collisions with enemies reduce life, and the game ends once life reaches zero.

Inspired by classic arcade titles such as *Monster Dash* and *Jetpack Joyride*, *Space Outage* blends reflex-based gameplay with modern shooting mechanics and cinematic storytelling.

---

## Gameplay Overview

### Start
- A short animated intro video shows an astronaut landing on an unknown planet after a spacecraft failure.
- Alien creatures begin attacking immediately after the landing sequence.

### Objective
- Survive as long as possible
- Collect coins
- Access alternate worlds through portal mechanics

### Controls
| Action | Key |
|------|-----|
| Jump | `SPACE` |
| Slide | `S` or `DOWN Arrow` |
| Shoot | `Left Mouse Click` + Mouse Aim |

### Special Mechanic
- Filling the coin bar opens a **temporary portal** to a new world with different visuals and gameplay variations.

### Death Condition
- Player life decreases upon collision with enemies
- Game ends when life reaches zero

---

## Technical Details

- **Programming Language:** C (C99)
- **Graphics & Audio Library:** Raylib
- **Platform:** Windows
- **IDE:** Visual Studio Code
- **Assets:** Royalty-free sprites, animations, and sound effects

---

## Game Loop Design

```c
while (!WindowShouldClose())
{
    CheckPlayerInput();           // Jump, Slide, Shoot
    UpdatePlayer();               // Position, animation
    SpawnAliens();                // Add new enemies
    CheckCollisions();            // Alien hits player?
    UpdateScoreAndCoins();        // Add coins, update UI
    DrawEverything();             // Background, player, UI
}

CloseWindow();  // Cleanup
```

---

## Code Structure

```
src/
├── main.c            // Game initialization and main loop
├── player.c / .h     // Player movement, state, shooting, animations
├── enemy.c / .h      // Alien behavior and spawning logic
├── world.c / .h      // World backgrounds and portal mechanics
├── ui.c / .h         // Score display, life bar, transitions
├── video_story.c / .h// Intro cinematic handling
└── assets/           // Images, sounds, music
```

---

## Features

- 🎬 Cinematic video intro
- 🏃 Responsive player movement (jump, slide, dodge)
- 🔫 Mouse-aimed shooting with angle control
- 🪙 Coin collection and scoring system
- 🌌 World-switching via portal mechanic
- ❤️ Life bar and damage system
- ⏸️ Pause, Game Over, and Respawn screens
- 🔊 Sound effects and background music

---

## Limitations

- No online leaderboard
- No save/load system
- No joystick/controller support
- Limited number of worlds
- No extended story progression

---

## Future Improvements

- Character upgrades and weapon unlocks
- Boss enemies
- Power-up system (shield, double coins, magnet)
- Save system and high-score tracking
- Procedural/random level chunks
- Full storyline implementation

---

## References & Inspiration

The gameplay mechanics and pacing were inspired by classic arcade-style endless runner games such as:
- *Monster Dash*
- *Jetpack Joyride*

---

## Screenshots

*(Screenshots to be added)*

---

**Author:** Nafis Habib  
**Project Type:** Academic / Game Development Project  
**Engine:** Raylib (C)
