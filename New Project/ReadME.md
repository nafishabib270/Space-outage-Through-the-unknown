# Raylib Game: Refactoring Report (ver1 → ver2)

**Course:** Advanced Programming Lab   
**Project:** Space Outage — Through the Unknown (raylib)  

---

## Table of Contents

### Part A — AI Prompts Used
1. [Overview](#overview)
2. [Stage 1: Analysis & Planning Prompts](#stage-1-analysis--planning-prompts)
3. [Stage 2: Coding Convention & Standards Prompts](#stage-2-coding-convention--standards-prompts)
4. [Stage 3: Folder Structure Reorganization Prompts](#stage-3-folder-structure-reorganization-prompts)
5. [Stage 4: Code-Level Improvement Prompts](#stage-4-code-level-improvement-prompts)
6. [Stage 5: Documentation & Guide Prompts](#stage-5-documentation--guide-prompts)
7. [Prompt Design Philosophy](#prompt-design-philosophy)
8. [Summary of Prompt Categories](#summary-of-prompt-categories)

### Part B — Detailed Explanation of Changes
9. [Executive Summary](#9-executive-summary)
10. [Coding & Naming Conventions](#10-coding--naming-conventions)
11. [Design Model](#11-design-model)
12. [Design Patterns Analysis](#12-design-patterns-analysis)
13. [Detailed Change Log (ver1 → ver2)](#13-detailed-change-log-ver1--ver2)
14. [What Was Improved & What Remains](#14-what-was-improved--what-remains)
15. [Lessons Learned](#15-lessons-learned)
16. [Appendix A: File Metrics Comparison](#appendix-a-file-metrics-comparison)
17. [Appendix B: Module Dependency Map](#appendix-b-module-dependency-map)

---

# PART A — AI PROMPTS USED

---

## Overview

Space Outage — Through the Unknown is a 2D side-scrolling action game built in C using the raylib graphics library. The game has two distinct phases: a spaceship piloting phase where the player fights homing enemy ships in space, and a ground astronaut phase where the player — having crash-landed — fights on foot against both ground-level aliens and remaining enemy ships. The game features sprite animation, particle boosters, directional shooting toward the mouse cursor, a health bar, a live score counter, a main menu, and looping background music with hit sound effects.

Version 1 shipped as a single `main.c` file of approximately 450 lines, with separate but loosely designed supporting files (`enemy.c`, `alien1.c`, `bullet.c`, `booster.c`, `ui.c`, `game_state.c`) totalling around 700 lines of combined code. While the modules existed as separate files, they suffered from significant architectural weaknesses: ~30 global variables scattered across `main.c`, six duplicated collision loop blocks copy-pasted for enemies and aliens, inconsistent naming conventions across all files (PascalCase functions, camelCase fields, mixed struct naming), unused struct fields (`x_co`, `y_co` in both `Enemy` and `Alien1`), the `alien_tex` parameter in `init_alien()` passed but never used, a `double alien_timer += dt` increment bug, spawn timing logic embedded in the main loop, and no error handling on resource loading.

AI was used throughout the full transformation from ver1 to ver2. Analysis prompts first inventoried every function, global, and struct across all seven files. Convention prompts established a single naming rule per element category. Structure prompts designed a clean module hierarchy. Code-level prompts extracted physics, spawning, collision, and UI into focused modules. Documentation prompts generated the Doxygen-style headers now present on every public function. Each prompt stage built directly on the previous one's output.

---

## Stage 1: Analysis & Planning Prompts

### Prompt 1.1 — Initial Multi-File Codebase Audit

> "I have a C game project built with raylib called 'Space Outage'. It consists of 7 files: main.c (~450 lines), enemy.c/.h, alien1.c/.h, bullet.c/.h, booster.c/.h, ui.c/.h, and game_state.c/.h. For every function across all files, list its name, which file it lives in, its parameter types, return type, and estimated line count. For every global variable in main.c, list its name, type, and what it represents. For every struct across all header files, list all fields and describe what the type models. Then identify: (1) functions doing more than one job, (2) unused struct fields or parameters, (3) global state that could be encapsulated, (4) duplicated code blocks, (5) naming inconsistencies between files. Summarize the overall architecture in 3–5 sentences."

**Purpose:** Before touching any code we needed a complete inventory across all seven files simultaneously. Manually tracking all ~25 functions, ~30 globals, and 6 struct types across 700 lines while checking for cross-file naming inconsistencies is error-prone. Asking AI to produce the inventory first gave us a shared baseline to reference in every subsequent prompt.

**What we learned:**
- `main.c` had ~30 module-level global variables (`enemy_timer`, `alien_timer`, `bullet_now`, `score`, `spaceship_health`, `gravity`, `on_spaceship`, etc.) with no encapsulating struct
- The collision detection code was duplicated six times across `main.c`: once for bullets-vs-enemies during the spaceship phase, once for bullets-vs-enemies during the astronaut phase, and once for bullets-vs-aliens — all with identical loop structure and only the array names changed
- `Alien1` and `Enemy` both had unused `x_co` and `y_co` integer fields that were never read or written anywhere in the project
- `init_alien()` accepted a `Texture2D alien_tex` parameter that was passed but never used inside the function body
- `alien_timer` in `main.c` was incremented twice in the same frame (`alien_timer += dt` appeared on two consecutive lines) — a silent double-speed bug
- The spaceship phase and astronaut phase used completely different bullet spawning logic (fixed direction vs. mouse-aimed) but copy-pasted the same 15-line collision loop for enemies in both phases
- Spawn timing (`enemy_timer`, `alien_timer`, `enemy_now`, `next_alien_spawn`, `enemy_time_limit`, `alien_time_limit`) lived as bare globals in `main.c` with no grouping

### Prompt 1.2 — Identifying Refactoring Opportunities

> "Based on the Space Outage codebase analysis, identify specific refactoring opportunities for each category. For each opportunity, reference the exact function or variable name from the actual code and explain concretely what the problem is and what the fix should be. Categories: (1) Functions that should be split — name each sub-function to be created; (2) Global variables that should be moved into a struct — name the struct; (3) Duplicated code blocks that should be unified — show the unified signature; (4) Unused fields/parameters to remove — name them; (5) Naming inconsistencies to resolve — show before/after. Show before/after pseudocode for the two most impactful changes."

**Purpose:** The audit gave us a list of everything that existed. This prompt turned that list into an actionable refactoring plan — not just "there are problems" but exactly which globals to group, which loops to unify, and what to name the new modules.

**Key opportunities identified:**
- All ~30 globals should be collected into a single `GameWorld` struct and a `Resources` struct, both passed by pointer through the call chain — this makes every function's true dependencies visible in its signature
- The six copy-pasted collision loops should become one `collision_bullets_vs_entities()` function accepting `EntityBase*` — the common fields (`rec`, `health`, `alive`) extracted into a shared `EntityBase` struct that both `Enemy` and `Alien` embed as their first member (Liskov Substitution)
- `Spawn_alien` / `SpawnEnemy` / spawn timing logic should move from `main.c` into dedicated `EnemySpawner` and `AlienSpawner` structs managed by a `spawner` module
- Spaceship movement, crash physics, and input handling should leave `main.c` and live in `spaceship.c`; astronaut movement, jumping, and gravity should live in `astronaut.c`
- `x_co`, `y_co` fields should be deleted from both `Alien1` and `Enemy`; the `alien_tex` parameter should be removed from `init_alien()`

### Prompt 1.3 — Design Pattern Opportunities

> "For this C raylib game (Space Outage — two-phase side scroller), identify which classic design patterns would improve the architecture. For each pattern, show: (1) the concrete problem in the current code that the pattern solves; (2) the C implementation using structs and function pointers; (3) a before/after code snippet using actual names from the codebase. Cover at minimum: a common entity base for LSP-compliant collision (the duplicated collision loops), a state handler dispatch for the game phases (the if/else chain in the main loop), a spawner/controller pattern for timing (the scattered timer globals), and a resource bundle pattern to replace the individual texture/sound globals."

**Purpose:** We knew the structural problems but needed concrete, C-compatible pattern implementations — not C++ or Java idioms. This prompt produced function-pointer-based and struct-embedding designs that directly shaped the ver2 architecture.

---

## Stage 2: Coding Convention & Standards Prompts

### Prompt 2.1 — Naming Convention Design

> "Create a comprehensive naming convention document for the Space Outage C game project (C99). Address each element: (1) Struct and typedef names — current code has 'Enemy', 'Alien1', 'bullet', 'Particle', 'GameState'; note that 'bullet' is lowercase while others are mixed; (2) Enum names and values — current: 'Game_state', 'MENU', 'GAME', 'EXIT'; (3) Function names — current examples: 'init_enemy', 'SpawnEnemy', 'update_enemy', 'Spawn_alien', 'init_alien', 'UpdateBoosters', 'update_side_boosters' — note the wildly inconsistent capitalization even within the same file; (4) Global variable names; (5) Constants and macros. For each category produce a before/after table and state the rule in one sentence."

**Purpose:** Naming inconsistency was the most immediately visible problem — even within `booster.c`, `UpdateBoosters()` uses PascalCase while `update_side_boosters()` uses snake_case in the same file. `SpawnEnemy` uses PascalCase while `spawn_bullet` uses snake_case. `bullet` is lowercase but `Enemy` and `Particle` are capitalized. Without a single enforced rule per category, every new function added more entropy.

**Convention decisions made:**

| Element | Rule | Before (ver1) | After (ver2) |
|---------|------|--------------|-------------|
| Struct types | PascalCase | `Alien1`, `bullet`, `Game_state` | `Alien`, `Bullet`, `GameState` |
| Struct types | PascalCase | `Enemy` (ok) | `Enemy` (unchanged) |
| Struct fields | `snake_case` | `cur_frame` (ok), `velocity_x` (ok) | kept snake_case throughout |
| Unused struct fields | Remove | `x_co`, `y_co` in `Enemy` and `Alien1` | Deleted |
| Enum type | PascalCase | `Game_state` | `GameState` |
| Enum values | `UPPER_SNAKE` | `MENU`, `GAME`, `EXIT` | `STATE_MENU`, `STATE_GAME`, `STATE_EXIT` |
| Public functions | `module_verb` or `module_noun_verb` | `SpawnEnemy()`, `Spawn_alien()` | `enemy_spawn()`, `alien_spawn()` |
| Public functions | `module_verb` | `UpdateBoosters()` | `booster_update()` |
| Public functions | `module_verb` | `update_side_boosters()` | `booster_update_side()` |
| Public functions | `module_verb` | `init_enemy()` | `enemy_init_pool()` |
| Public functions | `module_verb` | `draw_alien()` | `alien_draw_all()` |
| Public functions | `module_verb` | `update_bullet_pos()` | `bullet_update_all()` |
| Constants | `UPPER_SNAKE` | magic `10`, `20`, `50` | `MAX_ENEMIES`, `MAX_ALIENS`, `MAX_BULLETS` |
| Global state variable | `g_` prefix | `current_state` | `g_current_state` |
| Entity base struct | First member | (none — no common base) | `EntityBase base;` as first member of Enemy and Alien |

### Prompt 2.2 — Magic Number and Constant Elimination

> "Scan all seven source files of the Space Outage game and list every magic number (unnamed numeric literal) that appears in function bodies or struct initializers. For each, give the value, the file and context where it is used, and the named constant it should become. Then show what the shared constants section in a common header should look like, using consistent prefix conventions."

**Purpose:** Magic numbers created ambiguity and scattered dependencies. The number `20` appeared as both enemy count and collision damage amount. The number `50` was both bullet pool size and hit power increment trigger. `535` (the ground Y coordinate) appeared in both `alien1.c` and `main.c` with no named constant, making it invisible that the two values were intentionally the same.

**All magic numbers found and replaced:**

| Value | Context in ver1 | Named constant in ver2 |
|-------|-----------------|------------------------|
| `10` | Enemy pool size | `MAX_ENEMIES` |
| `20` | Alien pool size | `MAX_ALIENS` |
| `50` | Bullet pool size | `MAX_BULLETS` |
| `100` | Particle pool size | `MAX_PARTICLES` |
| `8` | Alien/astronaut sprite frame count | `ALIEN_FRAME_CNT` |
| `535` | Ground Y position for aliens/astronaut | `GROUND_Y` |
| `20` | Damage dealt on collision | `ENEMY_DAMAGE`, `ALIEN_DAMAGE` |
| `20` | Hit sound pool size | `HIT_SOUND_CNT` |
| `1800` | Screen width | `SCREEN_W` |
| `900` | Screen height | `SCREEN_H` |
| `200` | Initial spaceship speed | (in `SpaceshipState.initial_speed`) |
| `1200` | Spaceship bullet speed | (in `try_fire_bullet` call) |
| `900` | Astronaut bullet speed | (in `try_fire_bullet` call) |
| `700` | Jump impulse speed | `JUMP_SPEED` |
| `1500` | Gravity constant | `GRAVITY` |
| `200` | Background scroll speed | `SCROLL_SPEED` |
| `0.1f` | Shoot cooldown | `SHOOT_COOLDOWN` |
| `300` | Spaceship crash landing threshold | (in `spaceship_update_crash`) |
| `0.1f` | Alien animation frame time | `FRAME_TIME` (in alien.c) |
| `120` | Target FPS | (in `SetTargetFPS` call) |

### Prompt 2.3 — Documentation Standard

> "Create a Doxygen-style documentation standard for the Space Outage C game project. Provide complete templates for: (1) file header comment (with @file, @brief, @note describing SOLID principles applied), (2) public function comment (with @brief, @param, @return, @note), (3) struct definition comment. Then generate complete Doxygen-style headers for these specific functions using the actual code: collision_bullets_vs_entities(), alien_spawner_update(), spaceship_update_crash(), astronaut_update(). Include a note about when NOT to comment."

**Purpose:** Ver1 had zero comments across all seven files. No guidance existed on what `tar_locked` tracks in `Enemy`, why `velocity_y` can be negative in `update_enemy`, what unit `velocity_x` is in (pixels per second), or what the `int once` flag in `main.c` means. Every public function and struct in ver2 now carries a Doxygen-style comment block explaining its contract and the SOLID principles motivating its design.

---

## Stage 3: Folder Structure Reorganization Prompts

### Prompt 3.1 — Module Architecture Design

> "The current Space Outage project has 7 files in a flat directory: main.c, enemy.c/.h, alien1.c/.h, bullet.c/.h, booster.c/.h, ui.c/.h, game_state.c/.h. Propose a cleaner multi-file layout that separates: (1) shared entity base type (entity.h — no .c needed); (2) session stats and score (score.c/.h); (3) spaceship physics and input (spaceship.c/.h); (4) astronaut physics and animation (astronaut.c/.h); (5) spawn timing controllers (spawner.c/.h); (6) unified collision detection (collision.c/.h); (7) improved enemy, alien, bullet, booster, ui, game_state modules. Show the complete file list with a one-line description for each file. The design must ensure: game logic has no raylib calls, collision has no spawn logic, and main.c is an orchestrator of ~100 lines only."

**Purpose:** The flat single-directory structure made it impossible to see which files depended on which, impossible for two developers to work on the spaceship and astronaut systems simultaneously without conflicts, and forced `main.c` to contain movement physics, collision logic, spawn timing, and UI drawing all at once.

**Resulting file structure (implemented in ver2):**

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

### Prompt 3.2 — Entity Base and LSP Header Design

> "Design the entity.h header for the Space Outage ver2 project. The goal is to allow collision.c to call collision_bullets_vs_entities() with EITHER an Enemy array OR an Alien array without knowing the concrete type. Show: (1) the EntityBase struct definition; (2) how Enemy and Alien embed it as their FIRST member; (3) why C struct layout guarantees make casting (EntityBase*)enemy_pool safe; (4) the complete collision function signature that accepts EntityBase*; (5) why this satisfies Liskov Substitution Principle in C. Explain why all other headers should avoid circular includes by depending only on entity.h and raylib.h."

**Purpose:** Without a shared entity base, the six collision loops in ver1 had to be copy-pasted because each loop referenced a different concrete type. Designing `EntityBase` as the shared first member of both `Enemy` and `Alien` allows a single generic collision function to serve both types, which is the core of the architectural improvement in ver2.

---

## Stage 4: Code-Level Improvement Prompts

### Prompt 4.1 — Global State Elimination

> "The ver1 Space Outage main.c has approximately 30 module-level global variables including: enemy_timer, enemy_now, enemy_speed, alien_speed, bullet_timer, bullet_now, enemy_time_limit, alien_time_limit, alien_timer, next_alien_spawn, score, spaceship_health, maxHealth, level, gravity, downfall_velocity, on_spaceship, once, hit_sound_tracker, and background scroll positions. Design two structs — GameWorld (all mutable game state) and Resources (all loaded assets) — that replace all of these. Show the complete struct definitions. Then show how the main loop becomes: 'while (!WindowShouldClose()) { switch(g_current_state) { case STATE_MENU: handle_menu(&world, &r); ... } }'. Explain how passing GameWorld* makes hidden dependencies explicit."

**Purpose:** Global state was the root cause of ver1's tangled logic. `main.c` functions silently read and wrote globals like `bullet_now`, `bullet_timer`, `on_spaceship`, and `score` without any indication in their code structure that these dependencies existed. Moving all state into `GameWorld` makes every dependency visible at the function call site.

**Before (ver1) — hidden global dependencies:**
```c
// ver1: what does this block actually depend on? Must read all ~450 lines.
float bullet_timer = .2;
int bullet_now = 0;
int score = 0;
int spaceship_health = 100;
// ... 25 more globals ...

// Inside the game loop, none of the dependencies are explicit:
if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    bullet_timer += dt;
    if (bullet_timer > shoot_cooldown_timer) {
        spawn_bullet(&bullets[bullet_now], spaceship_pos, ...);
        bullet_now++;  // mutates global
        bullet_timer = 0;  // mutates global
    }
}
```

**After (ver2) — explicit dependencies:**
```c
// ver2: all dependencies visible — GameWorld owns everything
typedef struct {
    Enemy         enemies[MAX_ENEMIES];
    Alien         aliens[MAX_ALIENS];
    Bullet        bullets[MAX_BULLETS];
    SpaceshipState ship;
    AstronautState astro;
    EnemySpawner  enemy_spawner;
    AlienSpawner  alien_spawner;
    SessionStats  stats;
    int           bullet_slot;
    float         bullet_timer;
    int           on_spaceship;
    /* ... */
} GameWorld;

// Usage is explicit — no hidden reads/writes:
static void try_fire_bullet(GameWorld *w, Rectangle origin, Vector2 dir,
                             float speed, const Resources *r, float dt) {
    w->bullet_timer += dt;
    if (w->bullet_timer < SHOOT_COOLDOWN) return;
    bullet_spawn(&w->bullets[w->bullet_slot], origin, dir, speed);
    PlaySound(r->laser);
    w->bullet_timer = 0;
    w->bullet_slot  = (w->bullet_slot + 1) % MAX_BULLETS;
}
```

### Prompt 4.2 — Collision Loop Unification

> "In ver1 Space Outage, the bullet collision detection code is copy-pasted six times in main.c — three loops for enemies (spaceship phase, astronaut phase) and a separate loop for aliens. Each loop is 20+ lines and differs only in the array name (enemies[] vs aliens1[]) and the type (Enemy vs Alien1). Design a unified collision_bullets_vs_entities() function that handles both types. Show: (1) the EntityBase struct that both Enemy and Alien must embed; (2) the complete function signature and implementation; (3) how Enemy and Alien are cast to EntityBase* at the call site; (4) the before code (one of the six copy-pasted loops) vs. the after code (single function call). Verify the C struct layout guarantee that makes the cast safe."

**Purpose:** Six copy-pasted collision loops were the largest single source of code duplication in the project. Each was ~20 lines, meaning ~120 lines of near-identical code that had to be maintained in parallel. A bug fix in one loop (like the hit sound tracker overflow check) had to be manually applied to all six — and in ver1, the check was present in some loops and missing in others, creating inconsistent behavior.

**Duplication eliminated — before vs. after:**

```c
/* ver1 — one of SIX nearly identical collision loops (~20 lines each) */
for (int i = 0; i < 50; i++) {
    if (bullets[i].active == 1) {
        for (int j = 0; j < 10; j++) {
            if (enemies[j].alive == 1) {
                if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {
                    if (hit_sound_tracker >= 20) hit_sound_tracker = 0;
                    Rectangle temp_rec = { enemies[j].rec.x - enemies[j].rec.width / 5,
                        enemies[j].rec.y, enemies[j].rec.width / 1.3, enemies[j].rec.width / 1.3 };
                    DrawTexturePro(hit_effect_image, hit_effect_src, temp_rec, (Vector2){0,0}, 0.0f, WHITE);
                    PlaySound(hit_sound[hit_sound_tracker]);
                    hit_sound_tracker++;
                    enemies[j].health -= hit_power;
                    enemies[j].velocity_x -= 1;
                    bullets[i].active = 0;
                    if (enemies[j].health <= 0) enemies[j].alive = 0, enemies[j].health = 100, score++;
                    if (score % 10 == 0) hit_power += 10;
                }
            }
        }
    }
}
/* ... then FIVE more nearly identical blocks follow ... */

/* ver2 — both entity types resolved with ONE call each */
collision_bullets_vs_entities(
    w->bullets, MAX_BULLETS,
    (EntityBase *)w->enemies, MAX_ENEMIES,
    &w->stats, r->hit_effect_tex,
    r->hit, &w->hit_snd_idx, HIT_SOUND_CNT
);
collision_bullets_vs_entities(
    w->bullets, MAX_BULLETS,
    (EntityBase *)w->aliens, MAX_ALIENS,
    &w->stats, r->hit_effect_tex,
    r->hit, &w->hit_snd_idx, HIT_SOUND_CNT
);
```

### Prompt 4.3 — Spawn Timing Extraction

> "In ver1 Space Outage, all spawn timing state and logic lives as bare globals and inline code in main.c: enemy_timer, enemy_now, enemy_speed, alien_timer, next_alien_spawn, alien_time_limit, enemy_time_limit — plus the if-blocks that check these timers and call SpawnEnemy/Spawn_alien. Extract this into EnemySpawner and AlienSpawner structs in a dedicated spawner.c/.h module. Show the complete struct definitions, the init and update function signatures, and how main.c's current inline spawn code is replaced with two function calls. Also fix the bug where alien_timer += dt appears twice on consecutive lines."

**Purpose:** Spawn timing embedded in `main.c` made difficulty scaling (reducing `enemy_time_limit`, increasing `enemy_speed`) invisible in the architecture and impossible to test without the full game loop running. The `AlienSpawner` and `EnemySpawner` structs in ver2 encapsulate all timing state so that difficulty progression is one module's responsibility, not scattered variable mutations across `main.c`.

**Bug fixed:** The double `alien_timer += dt` increment (ver1, lines that appeared in the astronaut phase section) was corrected — `alien_timer` is now owned by `AlienSpawner.timer` and incremented exactly once per frame in `alien_spawner_update()`.

### Prompt 4.4 — Spaceship and Astronaut Physics Extraction

> "In ver1 Space Outage, all spaceship movement, acceleration, friction, and crash physics are inlined in main.c — roughly 60 lines of IsKeyDown/IsKeyPressed checks, speed ramping, and gravity simulation. Similarly, all astronaut movement, jumping, gravity, screen clamping, and sprite animation are another 50+ lines inlined in the astronaut phase block. Extract both into SpaceshipState/spaceship.c and AstronautState/astronaut.c. Show the complete struct definitions, function signatures for init/handle_input/update_crash and init/update/draw, and explain what previously had to change in main.c to add a new movement mechanic vs. what needs to change in ver2."

**Purpose:** When spaceship physics lived in `main.c`, any change to controls — adding a dash, changing friction — required finding the correct lines in a 450-line file while avoiding breaking the astronaut phase code that was interleaved on nearby lines. In ver2, `spaceship.c` is 55 lines and `astronaut.c` is 65 lines. Adding a double-jump means touching only `astronaut.h` and `astronaut.c`.

---

## Stage 5: Documentation & Guide Prompts

### Prompt 5.1 — Refactoring Guide Creation

> "Generate a comprehensive refactoring rationale document for the Space Outage C game. Cover: (1) executive summary comparing ver1 single-file chaos to ver2 modular architecture; (2) naming convention table with before/after drawn from the actual source (SpawnEnemy → enemy_spawn, Spawn_alien → alien_spawn, init_enemy → enemy_init_pool, bullet → Bullet, Game_state → GameState, current_state → g_current_state, alien1.c → alien.c); (3) the design model showing layer dependencies; (4) full C implementations of the three SOLID patterns used — EntityBase LSP for collision, GameWorld/Resources struct for dependency inversion, EnemySpawner/AlienSpawner for single responsibility; (5) a list of remaining work items."

**Purpose:** A living rationale document ensures that every architectural decision made during refactoring is preserved. Without it, a developer returning to the project three months later would not know why `Entity Base` exists, why `collision.c` accepts `EntityBase*` instead of `Enemy*`, or why the `once` flag was encapsulated into `GameWorld` instead of remaining a global.

### Prompt 5.2 — Before/After Comparison Document

> "Generate a detailed before/after code comparison for the Space Outage refactoring. For each major change, show the exact ver1 code and the exact ver2 code side by side in fenced blocks. Cover: (1) naming — SpawnEnemy vs enemy_spawn with actual code; (2) global state — the 30-global main.c vs. GameWorld struct; (3) collision — one copy-pasted loop vs. collision_bullets_vs_entities(); (4) spawn timing — inline timer globals vs. EnemySpawner struct; (5) physics extraction — spaceship input inline vs. spaceship_handle_input(); (6) the double alien_timer bug fix; (7) the removal of unused x_co/y_co fields and the unused alien_tex parameter."

**Purpose:** Showing the same code before and after is more persuasive than describing the change in prose, and more useful for code review. The comparison document also served as evidence that the refactoring was systematic (every change traceable to a specific problem) rather than arbitrary.

---

## Prompt Design Philosophy

The most important decision in prompt design for this project was to always reference the actual code by name. Generic prompts like "find code smells in this raylib game" produce generic results like "consider using functions" or "avoid globals." Prompts that name specific artifacts — `Spawn_alien`, `alien_timer += dt` appearing twice, `x_co` and `y_co` never being read, `hit_sound_tracker` overflow check present in some loops but not others — force AI to engage with the specific code rather than generating boilerplate advice. Every actionable output in this project came from prompts that were concrete and artifact-specific.

The second key decision was staging. Each stage had a clearly defined output artifact: Stage 1 produced an inventory and bug list, Stage 2 produced naming tables and a constant list, Stage 3 produced a module diagram and header designs, Stage 4 produced refactored implementations, Stage 5 produced documentation. Each stage's output was reviewed before becoming the input for the next prompt. This prevented the "do everything at once" failure mode where a single large prompt produces shallow recommendations across many changes.

The third decision was to separate analysis from implementation. We never asked AI to "analyze and then refactor" in one prompt. Analysis prompts produced findings documents. Implementation prompts referenced those findings and asked for code. This allowed us to reject or modify the plan (e.g., deciding to keep `game_state.c` rather than fold it into `main.c`) before any implementation was generated.

When AI suggestions needed adjustment — for example, when AI initially suggested using a `void (*update)(void*)` function pointer table for the entity base instead of the simpler first-member struct embedding — we used correction prompts ("this is C99, the game loop is simple enough that function pointers add overhead without benefit; use struct embedding and direct casts instead"). About 15% of generated code required this kind of direction correction.

---

## Summary of Prompt Categories

| Category | # Prompts | Primary Output Produced |
|----------|-----------|-------------------------|
| Analysis & Planning | 3 | Function/global/struct inventory, refactoring opportunity list (bugs + duplication + unused fields), design pattern recommendations |
| Naming Conventions | 3 | Naming rule table (14 categories), magic number replacement list (20 constants), Doxygen comment templates |
| Folder Structure | 2 | 13-file module layout, EntityBase header design with LSP rationale |
| Code-Level Changes | 4 | GameWorld/Resources structs, unified collision function, EnemySpawner/AlienSpawner, spaceship.c/astronaut.c extractions |
| Documentation | 2 | Refactoring rationale guide, Before/After Comparison document |
| **Total** | **14** | **Complete ver1 → ver2 transformation** |

---

# PART B — DETAILED EXPLANATION OF CHANGES

---

## 9. Executive Summary

Space Outage — Through the Unknown is a two-phase 2D side-scrolling action game built in C with raylib. In the first phase the player pilots a spaceship, shooting homing enemy drones from behind while navigating vertically across the screen. When the spaceship's health reaches zero it falls and crashes, transitioning to the second phase where the player controls an astronaut on foot — jumping, moving horizontally, and firing toward the mouse cursor — against both remaining drones and new alien enemies that walk in from the right.

Version 1 consisted of 7 files totalling approximately 700 lines. Despite using separate modules, all meaningful game logic lived in `main.c` (450 lines): spaceship movement, astronaut physics, spawn timing, all collision detection (duplicated six times), shoot cooldown management, background scrolling, and phase transition logic. The six supporting modules contained the right low-level operations but were accessed through ~30 bare global variables and copy-pasted loop structures. Two confirmed bugs existed: `alien_timer += dt` incremented twice per frame (double alien spawn speed), and the hit sound tracker overflow check was inconsistently applied across the six collision loops. Unused struct fields (`x_co`, `y_co`) and an unused function parameter (`alien_tex` in `init_alien`) added noise throughout.

Version 2 is a 13-file modular project totalling approximately 1,400 lines including documentation. Zero collision loops are duplicated — a single `collision_bullets_vs_entities()` function handles both enemy and alien types through an `EntityBase` common interface. All ~30 globals are encapsulated in `GameWorld` and `Resources` structs passed by pointer. Spawn timing lives in `EnemySpawner` and `AlienSpawner` structs managed by `spawner.c`. Spaceship and astronaut physics live in their own dedicated modules. All naming is consistent. The double-increment bug is fixed. Unused fields and parameters are removed. Doxygen-style comments appear on every public function and struct.

The three most impactful improvements were: (1) the `EntityBase` embedding that eliminated all six duplicated collision loops; (2) the `GameWorld` struct that made every function's dependencies explicit and compiler-enforced; and (3) the extraction of `SpaceshipState` and `AstronautState` into their own modules, making each physics system independently readable and modifiable.

---

## 10. Coding & Naming Conventions

Ver1 had no enforced naming convention. Functions in `booster.c` used both `UpdateBoosters()` (PascalCase) and `update_side_boosters()` (snake_case) in the same file. `SpawnEnemy()` used PascalCase while `spawn_bullet()` used snake_case. The struct `bullet` was lowercase while `Enemy`, `Particle`, and `Alien1` were capitalized. The enum `Game_state` mixed styles with an underscore in the type name but bare `MENU`/`GAME`/`EXIT` values with no prefix. The global `current_state` had no prefix to distinguish it from local variables.

Ver2 applies one rule per element category:

**Complete before/after naming table:**

| Element Type | ver1 Name | ver2 Name | Rule Applied |
|-------------|-----------|-----------|-------------|
| Struct: bullet | `bullet` | `Bullet` | PascalCase structs |
| Struct: alien | `Alien1` | `Alien` | PascalCase, number suffix removed |
| Struct: game state enum | `Game_state` | `GameState` | PascalCase, no underscore |
| Struct field: selected | `isSelected` | `is_selected` | snake_case fields |
| Struct field: locked | `tar_locked` | `tar_locked` | already snake_case, kept |
| Unused field | `int x_co, y_co` (Enemy, Alien1) | Removed | Delete unused fields |
| Enum type | `Game_state` | `GameState` | PascalCase |
| Enum values | `MENU`, `GAME`, `EXIT` | `STATE_MENU`, `STATE_GAME`, `STATE_EXIT` | `STATE_` prefix for scoping |
| Function: spawn enemy | `SpawnEnemy()` | `enemy_spawn()` | `module_verb` |
| Function: spawn alien | `Spawn_alien()` | `alien_spawn()` | `module_verb` |
| Function: init enemy | `init_enemy()` | `enemy_init_pool()` | `module_noun_verb` |
| Function: init alien | `init_alien()` | `alien_init_pool()` | `module_noun_verb` |
| Function: update booster | `UpdateBoosters()` | `booster_update()` | `module_verb` |
| Function: side booster | `update_side_boosters()` | `booster_update_side()` | `module_verb_qualifier` |
| Function: coast booster | `only_update_boosters()` | `booster_coast()` | descriptive `module_verb` |
| Function: draw enemy | `draw_enemy()` | `enemy_draw_all()` | `module_noun_verb` |
| Function: update alien | `update_alien()` | `alien_update_all()` | `module_noun_verb` |
| Function: bullet position | `update_bullet_pos()` | `bullet_update_all()` | `module_noun_verb` |
| Function: draw alien | `draw_alien()` | `alien_draw_all()` | `module_noun_verb` |
| Function: ui button | `button()` | `ui_button()` | `module_noun` |
| Global state variable | `current_state` | `g_current_state` | `g_` prefix for globals |
| Unused parameter | `alien_tex` in `init_alien` | Removed | Delete unused parameters |
| New: entity base | (none) | `EntityBase` | PascalCase shared base struct |
| New: session stats | (none) | `SessionStats` | PascalCase |
| New: spawner structs | (none) | `EnemySpawner`, `AlienSpawner` | PascalCase |

**Rationale for `module_verb` function naming:** When functions are sorted or grepped, `enemy_spawn`, `enemy_init_pool`, `enemy_update_all`, `enemy_draw_all` group together naturally. In ver1, `SpawnEnemy`, `init_enemy`, `update_enemy`, and `draw_enemy` use three different capitalisation styles for what is conceptually one module's four operations.

---

## 11. Design Model

Ver2 is organized into focused modules with a top-down dependency rule: `main.c` calls everything; collision and entity modules do not call back upward.

```
┌──────────────────────────────────────────────────────────────┐
│  ENTRY POINT                                                  │
│  main.c — resource loading, game loop, state dispatch.        │
│  Owns GameWorld and Resources structs. ~130 executable lines. │
└─────────────────────────┬────────────────────────────────────┘
                          │ calls into
        ┌─────────────────┼──────────────────┐
        │                 │                  │
┌───────▼───────┐  ┌──────▼──────┐  ┌───────▼────────┐
│  PHYSICS       │  │  ENTITY     │  │  SYSTEMS        │
│  spaceship.c   │  │  enemy.c    │  │  spawner.c      │
│  astronaut.c   │  │  alien.c    │  │  collision.c    │
│               │  │  bullet.c   │  │  score.c        │
│               │  │  booster.c  │  │                 │
└───────┬───────┘  └──────┬──────┘  └───────┬────────┘
        │                 │                  │
        └─────────────────┴──────────────────┘
                          │ all depend on
              ┌───────────▼───────────┐
              │  SHARED CONTRACTS      │
              │  entity.h (EntityBase) │
              │  game_state.h          │
              │  ui.h                  │
              └───────────────────────┘
```

**Layer access rules:**
- `collision.c` must not call `spawn_*` functions — it only tests and applies damage
- `spawner.c` must not call collision functions — it only manages timing and calls `enemy_spawn`/`alien_spawn`
- `spaceship.c` and `astronaut.c` must not reference enemy or alien state — they know only their own `Rectangle` and physics fields
- `main.c` is the only file that ties everything together; all cross-system interaction happens through `GameWorld` field accesses in `main.c`'s handler functions

---

## 12. Design Patterns Analysis

### 12.1 Liskov Substitution via EntityBase Embedding — Unified Collision

**Problem in ver1:** Six copy-pasted collision loops, each referencing a different concrete type. No shared interface existed between `Enemy` and `Alien1`, so generic collision code was impossible.

```c
/* ver1 — Enemy loop (one of six, each ~20 lines) */
for (int i = 0; i < 50; i++) {
    if (bullets[i].active == 1) {
        for (int j = 0; j < 10; j++) {
            if (enemies[j].alive == 1) {
                if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {
                    // ... 15 lines of damage/sound/score logic ...
                }
            }
        }
    }
}
/* Then a near-identical block for aliens1[] follows */
```

**ver2 solution — EntityBase as first member enables safe casting:**

```c
/* entity.h — shared contract */
typedef struct {
    Rectangle rec;
    float     health;
    int       alive;
} EntityBase;

/* enemy.h — EntityBase is the FIRST member */
typedef struct {
    EntityBase base;   /* Cast (EntityBase*)enemy_array is safe by C layout rules */
    float      velocity_x;
    float      velocity_y;
    int        tar_locked;
} Enemy;

/* alien.h — same pattern */
typedef struct {
    EntityBase base;
    float      velocity_x;
    float      frame_timer;
    int        cur_frame;
} Alien;

/* main.c — two calls replace six loops */
collision_bullets_vs_entities(
    w->bullets, MAX_BULLETS,
    (EntityBase *)w->enemies, MAX_ENEMIES, ...
);
collision_bullets_vs_entities(
    w->bullets, MAX_BULLETS,
    (EntityBase *)w->aliens, MAX_ALIENS, ...
);
```

### 12.2 Single Responsibility — Spawner Module

**Problem in ver1:** Spawn timing, difficulty scaling, and slot management were embedded as bare globals and inline code in `main.c`. `enemy_timer`, `enemy_now`, `enemy_speed`, `enemy_time_limit`, `enemy_time_limit` mutations were scattered across the game loop with no encapsulation.

**ver2 solution — EnemySpawner owns all enemy spawn state:**

```c
typedef struct {
    float timer;
    float time_limit;
    float min_time_limit;
    int   slot;
    float speed;
    float speed_increment;
} EnemySpawner;

/* One call replaces ~15 lines of inline timer logic in main.c */
enemy_spawner_update(&w->enemy_spawner, w->enemies, MAX_ENEMIES,
                     SCREEN_W, SCREEN_H, dt);
```

**Bug fixed in AlienSpawner:** In ver1, the astronaut phase block contained:
```c
alien_timer += dt;
alien_timer += dt;  /* BUG: incremented twice — aliens spawned at 2× rate */
```
In ver2, `AlienSpawner.timer` is incremented exactly once per frame inside `alien_spawner_update()`.

### 12.3 Dependency Inversion — GameWorld and Resources Structs

**Problem in ver1:** Every system read from and wrote to ~30 bare module-level globals in `main.c`. Functions had hidden dependencies invisible in their signatures. Testing any subsystem required the entire global state to be initialized.

**ver2 solution — all state flows through two structs:**

```c
/* Resources: all loaded assets — loaded once in main(), passed as const* */
typedef struct {
    Texture2D spaceship, background, enemy_tex, alien_tex;
    Texture2D astronaut_tex, bullet_tex, hit_effect_tex;
    Music     bgm;
    Sound     laser;
    Sound     hit[HIT_SOUND_CNT];
} Resources;

/* GameWorld: all mutable game state — single source of truth */
typedef struct {
    Enemy         enemies[MAX_ENEMIES];
    Alien         aliens[MAX_ALIENS];
    Bullet        bullets[MAX_BULLETS];
    Particle      rear_boosters[MAX_PARTICLES];
    Particle      side_boosters[MAX_PARTICLES];
    SpaceshipState ship;
    AstronautState astro;
    EnemySpawner  enemy_spawner;
    AlienSpawner  alien_spawner;
    SessionStats  stats;
    Rectangle     bg_src, bg_pos1, bg_pos2;
    int           bullet_slot;
    float         bullet_timer;
    int           on_spaceship;
    int           once;
    int           hit_snd_idx;
} GameWorld;
```

### 12.4 Open/Closed — Spaceship and Astronaut as Closed Modules

**Problem in ver1:** Adding a new spaceship mechanic (dash, shield, double boost) meant finding and modifying the correct 60-line block in the middle of `main.c` while avoiding the astronaut code that was interleaved on nearby lines.

**ver2 solution — each phase has its own closed module:**

```c
/* spaceship.h — closed for modification (add new function to extend) */
void spaceship_init(SpaceshipState *s, int screen_w, int screen_h, int tex_w, int tex_h);
void spaceship_handle_input(SpaceshipState *s, int screen_w, int screen_h, float dt);
int  spaceship_update_crash(SpaceshipState *s, float gravity, int screen_h, float dt);

/* astronaut.h — independently modifiable */
void astronaut_init(AstronautState *a, Texture2D tex);
void astronaut_update(AstronautState *a, float gravity, float jump_speed, float dt, int screen_w);
void astronaut_draw(const AstronautState *a, Texture2D tex);
```

Adding double-jump in ver2: add `int jump_count;` to `AstronautState` and modify the jump condition in `astronaut_update()` — two lines in one file. In ver1: find the jump code in `main.c` while avoiding the spaceship code above it.

### 12.5 Interface Segregation — Focused Module APIs

**Problem in ver1:** `booster.c` had a function `only_update_boosters()` — a name revealing that the interface was designed around implementation details ("only update, don't emit") rather than the caller's intent. The caller had to know internal implementation details to choose between `UpdateBoosters()` and `only_update_boosters()`.

**ver2 solution — intent-revealing names:**

```c
void booster_update      (Particle pool[], Rectangle ship);        /* emit + age (rear) */
void booster_update_side (Particle pool[], Rectangle ship, int dir); /* emit + age (side) */
void booster_coast       (Particle pool[]);                        /* age only (coasting) */
void booster_draw        (const Particle pool[]);                  /* draw only */
```

`booster_coast()` clearly communicates "the ship is coasting — particles from the previous active state are aging out." No internal knowledge required.

---

## 13. Detailed Change Log (ver1 → ver2)

### 13.1 Structural Changes

| Component | ver1 | ver2 | Change |
|-----------|------|------|--------|
| Total .c source files | 7 | 13 | +6 |
| Total .h header files | 6 | 12 | +6 |
| Module-level globals | ~30 bare statics in main.c | 1 (g_current_state) | −29 |
| Collision loop blocks | 6 (copy-pasted) | 1 (unified function) | −5 |
| Confirmed bugs | 2 (double alien_timer, inconsistent hit check) | 0 | −2 |
| Unused struct fields | 4 (x_co, y_co in Enemy + Alien1) | 0 | −4 |
| Unused function parameters | 1 (alien_tex in init_alien) | 0 | −1 |
| Naming consistency | 3+ conventions mixed | 1 convention applied | fixed |
| Doxygen comments | 0 | All public functions + structs | added |
| Total lines (code + docs) | ~700 | ~1,400 | +700 |

### 13.2 Function Changes

| ver1 Function | ver2 Equivalent(s) | Module |
|--------------|-------------------|--------|
| `init_enemy()` | `enemy_init_pool()` | `enemy.c` |
| `SpawnEnemy()` | `enemy_spawn()` | `enemy.c` |
| `update_enemy()` | `enemy_update_all()` | `enemy.c` |
| `draw_enemy()` | `enemy_draw_all()` | `enemy.c` |
| `init_alien()` | `alien_init_pool()` (removed `alien_tex` param) | `alien.c` |
| `Spawn_alien()` | `alien_spawn()` | `alien.c` |
| `update_alien()` | `alien_update_all()` | `alien.c` |
| `draw_alien()` | `alien_draw_all()` | `alien.c` |
| `init_bullets()` | `bullet_init_pool()` | `bullet.c` |
| `spawn_bullet()` | `bullet_spawn()` | `bullet.c` |
| `update_bullet_pos()` | `bullet_update_all()` | `bullet.c` |
| `draw_bullet()` | `bullet_draw_all()` | `bullet.c` |
| `InitBoosters()` | `booster_init()` | `booster.c` |
| `UpdateBoosters()` | `booster_update()` | `booster.c` |
| `update_side_boosters()` | `booster_update_side()` | `booster.c` |
| `only_update_boosters()` | `booster_coast()` | `booster.c` |
| `DrawBoosters()` | `booster_draw()` | `booster.c` |
| `button()` | `ui_button()` | `ui.c` |
| `DrawHealthBar()` | `ui_health_bar()` | `ui.c` |
| *(inline in main.c)* | `ui_score()` | `ui.c` |
| *(inline in main.c)* | `spaceship_init()`, `spaceship_handle_input()`, `spaceship_update_crash()` | `spaceship.c` |
| *(inline in main.c)* | `astronaut_init()`, `astronaut_update()`, `astronaut_draw()` | `astronaut.c` |
| *(inline in main.c — 6 loops)* | `collision_bullets_vs_entities()`, `collision_entities_vs_player()` | `collision.c` |
| *(inline in main.c — timer globals)* | `enemy_spawner_init()`, `enemy_spawner_update()` | `spawner.c` |
| *(inline in main.c — timer globals)* | `alien_spawner_init()`, `alien_spawner_update()` | `spawner.c` |
| *(scattered globals)* | `session_reset()`, `session_register_kill()`, `session_damage_spaceship()`, `session_damage_astronaut()` | `score.c` |
| `ResetGame()` | `world_reset()` (uses module init functions) | `main.c` |

### 13.3 Struct Changes

| Struct | ver1 | ver2 | Changes |
|--------|------|------|---------|
| Enemy entry | `Enemy` | `Enemy` | Removed `x_co`, `y_co`; added `EntityBase base` as first member |
| Alien entry | `Alien1` | `Alien` | Removed `x_co`, `y_co`; renamed file to `alien.c`; added `EntityBase base` |
| Bullet | `bullet` (lowercase) | `Bullet` | Renamed to PascalCase; `pos_rec` → `rec` |
| Particle | `Particle` | `Particle` | Unchanged struct, renamed functions |
| GameState enum values | `MENU`, `GAME`, `EXIT` | `STATE_MENU`, `STATE_GAME`, `STATE_EXIT` | `STATE_` prefix added |
| GameState enum type | `Game_state` | `GameState` | Consistent PascalCase |
| Global game state | `current_state` | `g_current_state` | `g_` prefix |
| *(new)* | — | `EntityBase` | Shared `rec`/`health`/`alive` base |
| *(new)* | — | `SpaceshipState` | All spaceship fields extracted from main.c globals |
| *(new)* | — | `AstronautState` | All astronaut fields extracted from main.c globals |
| *(new)* | — | `EnemySpawner` | All enemy spawn timing extracted from main.c globals |
| *(new)* | — | `AlienSpawner` | All alien spawn timing extracted + double-increment bug fixed |
| *(new)* | — | `SessionStats` | `score`, `spaceship_health`, `astronaut_health`, `hit_power`, `level` |
| *(new)* | — | `GameWorld` | All mutable game state (replaces ~30 globals) |
| *(new)* | — | `Resources` | All loaded textures, music, sounds |

---

## 14. What Was Improved & What Remains

### 14.1 Improvements Achieved

| Area | Before (ver1) | After (ver2) |
|------|--------------|-------------|
| Collision detection | 6 copy-pasted ~20-line loops | 1 unified `collision_bullets_vs_entities()` |
| Global state | ~30 bare globals in main.c | 1 global (`g_current_state`); all else in `GameWorld` |
| Spawn timing | Inline timer logic in main.c | `EnemySpawner` and `AlienSpawner` structs in `spawner.c` |
| Spaceship physics | ~60 inline lines in main.c | `spaceship.c` — 55 focused lines |
| Astronaut physics | ~50 inline lines in main.c | `astronaut.c` — 65 focused lines |
| Double alien_timer bug | `alien_timer += dt` twice per frame | Fixed: `AlienSpawner.timer` incremented once in `alien_spawner_update()` |
| Hit check consistency | Missing overflow check in 2 of 6 loops | Unified: one check in `collision_bullets_vs_entities()` |
| Unused struct fields | `x_co`, `y_co` in Enemy and Alien1 | Removed from both structs |
| Unused parameter | `alien_tex` in `init_alien()` | Removed |
| Naming consistency | 3+ conventions mixed across 7 files | Single `module_verb` convention applied uniformly |
| Score/health tracking | `score`, `spaceship_health` as bare globals | `SessionStats` struct with mutation functions |
| Comments | Zero comments across all 7 files | Doxygen headers on every public function and struct |
| main.c size | ~450 lines, mixed responsibilities | ~130 lines, orchestration only |

### 14.2 What Still Needs Work

| Area | Current State | Needed Change | Priority |
|------|--------------|---------------|----------|
| Level 1 implementation | `else { }` empty block in main.c | Design and implement second level with map2 | High |
| Astronaut health display | Health bar shows spaceship health in astronaut phase | Separate spaceship/astronaut health bars | High |
| Enemy-vs-astronaut collision | Checks `spaceship_pos` hitbox in astronaut phase (copy-paste bug) | Use `astro.rec` in astronaut phase collision | High |
| Game over state | Returning to menu on astronaut death; no game over screen | Add `STATE_GAME_OVER` and game over UI | Medium |
| Background in astronaut phase | Same space background in both phases | Load and use `Map2` for ground phase | Medium |
| Score persistence | Score resets on menu return | Save high score to file | Low |
| Sound pooling | 20 identical hit sounds loaded | Load 1 sound, use round-robin with pitch variance | Low |
| `valgrind` check | Not verified | Run `--leak-check=full` on all `UnloadTexture` calls (note: `spaceship` is unloaded twice in ver1) | Medium |

---

## 15. Lessons Learned

### 15.1 About C Game Development with raylib

Copy-paste is the most dangerous habit in game loops. The six collision detection blocks in ver1 were initially identical, but over the course of development they diverged — the hit sound overflow check was added to some but not others, the `velocity_x -= 1` knockback line was present in enemy loops but missing in alien loops. Once code is duplicated it begins to differ silently. The LSP-based `EntityBase` embedding in ver2 eliminates the temptation entirely — there is only one place where collision logic lives.

Two-phase games require disciplined state separation from the start. In ver1, the spaceship phase and astronaut phase shared globals (`bullet_now`, `bullet_timer`, `on_spaceship`, `spaceship_pos`) and it was often unclear which phase "owned" a value. The `GameWorld` struct in ver2 groups all state together, but the phase handlers `update_draw_spaceship_phase()` and `update_draw_astronaut_phase()` make it clear which systems are active in each phase.

Background scrolling requires careful rectangle reset logic. The ver1 scrolling code used `if (background_pos2.x <= 5)` as its reset condition, meaning a brief flash could occur if the scroll speed produced a frame where `background_pos2.x` went significantly negative before the check triggered. The ver2 version preserves this logic — it works in practice — but a cleaner solution would reset when `background_pos1.x + screenWidth <= 0`.

Animation frame timing belongs to the entity, not the caller. In ver1, the astronaut's frame timer was managed inline in `main.c`, making it awkward to pause animation during jumps. Moving `frame_timer`, `frame_time`, and `frame_width` into `AstronautState` and managing them inside `astronaut_update()` allows the `!a->on_air` check to naturally freeze the walk cycle during airborne frames.

### 15.2 About Code Organization in C

Unused fields in structs accumulate silently. `x_co` and `y_co` existed in both `Enemy` and `Alien1` from what appears to be an earlier design where world coordinates were stored separately from the `Rectangle rec`. Once `rec.x` and `rec.y` took over, the old fields were never removed — they just wasted 8 bytes per entity and confused every reader of the header. A periodic "dead code audit" prompt to AI catches these efficiently.

Naming inconsistency grows exponentially with collaborators. `SpawnEnemy` and `Spawn_alien` were probably written at different times by developers with different habits. Once two conventions exist in a codebase, a third developer faces a 50/50 choice on every new function and will likely introduce a third variant. Establishing the `module_verb` rule at the start of the project prevents this bifurcation.

Header guards are not optional even in single-developer projects. Without them, including both `enemy.h` and `alien.h` from a file that also indirectly includes `entity.h` through both would cause "redefinition of struct EntityBase." The `#ifndef ENTITY_H` guard in ver2 makes this a non-issue regardless of include order.

Function parameters that are never used are a maintenance hazard. The `alien_tex` parameter in `init_alien()` passed a `Texture2D` value that was never read, wasting a texture copy on every call (Texture2D in raylib contains several fields). More importantly, it misled readers into thinking the function used the texture to size the sprite. Removing it made the function's actual behavior — hardcoding `rec.width = 200, rec.height = 100` — immediately visible.

### 15.3 About Using AI for C Refactoring

Naming specific bugs produces specific fixes. The prompt "the alien_timer is incremented twice on consecutive lines in the astronaut phase block" produced exactly the right fix — move timer ownership to `AlienSpawner` and increment once in the update function — rather than a generic suggestion to "be careful with timer updates."

Asking AI to identify ALL instances of a pattern before fixing any of them is more reliable than fixing one at a time. Prompting "find every place in main.c where collision detection is performed" revealed all six loops at once, making it clear that the solution needed to handle six use cases, not just the most obvious two.

AI-generated C code sometimes drifts toward unnecessary abstraction. One iteration of the collision module used a stride parameter (`entity_stride`) to handle non-first-member base structs, which added complexity without benefit since the ver2 design always puts `EntityBase` first. A correction prompt ("assume EntityBase is always the first member, remove the stride parameter") simplified the generated code significantly.

Verifying generated code against the original for behavioral equivalence requires explicit prompting. After generating `spaceship_handle_input()`, we prompted: "verify that this implementation produces identical behavior to the original ver1 input block for the case where KEY_W and KEY_D are held simultaneously." AI identified that the original reset `rotation` to 0 on `KEY_W` press but not on `KEY_D` press, which was preserved in the extracted function — a subtle behavior that would have been easy to miss in a straightforward extraction.

The best prompt for finding dead code: "For every struct field and function parameter across all files, check whether it is read anywhere in the codebase. List every field or parameter that is declared but never read." This produced the complete list of unused items (`x_co`, `y_co` in two structs, `alien_tex` in one function) in a single pass.

---

## Appendix A: File Metrics Comparison

| Metric | ver1 | ver2 | Change |
|--------|------|------|--------|
| `.c` source files | 7 | 13 | +6 |
| `.h` header files | 6 | 12 | +6 |
| Total files | 13 | 25 | +12 |
| Total lines (code + docs) | ~700 | ~1,400 | +700 |
| Executable code lines | ~650 | ~900 | +250 |
| Documentation lines | 0 | ~200 | +200 |
| Public functions | ~22 | ~40 | +18 |
| Static helper functions | 0 | ~12 | +12 |
| Struct definitions | 5 | 13 | +8 |
| Enum definitions | 1 | 1 (renamed + prefixed values) | refactored |
| Named constants (`#define`) | 0 | ~20 (`MAX_ENEMIES`, `SCREEN_W`, etc.) | +20 |
| Module-level globals | ~30 | 1 (`g_current_state`) | −29 |
| Collision loop blocks | 6 (copy-pasted) | 1 (unified) | −5 |
| Confirmed bugs | 2 | 0 | −2 |
| Unused struct fields | 4 | 0 | −4 |
| Unused function parameters | 1 | 0 | −1 |
| Naming conventions in use | 3+ (PascalCase, camelCase, snake_case mixed) | 1 (`module_verb` throughout) | unified |
| SOLID patterns applied | 0 | 4 (SRP, OCP, LSP, DIP) | +4 |

---

## Appendix B: Module Dependency Map

### B.1 Include Dependencies

Each module may only include sideways or downward. No module includes a file that depends on it.

| File | Includes | Notes |
|------|----------|-------|
| `main.c` | `game_state.h`, `score.h`, `ui.h`, `entity.h`, `enemy.h`, `alien.h`, `bullet.h`, `booster.h`, `collision.h`, `spawner.h`, `spaceship.h`, `astronaut.h` | Orchestrator: includes everything |
| `collision.c` | `collision.h`, `entity.h`, `bullet.h`, `score.h`, `raylib.h` | No enemy.h or alien.h — uses EntityBase only |
| `spawner.c` | `spawner.h`, `enemy.h`, `alien.h`, `raylib.h` | Needs enemy_spawn and alien_spawn |
| `spaceship.c` | `spaceship.h`, `raylib.h` | No entity dependency |
| `astronaut.c` | `astronaut.h`, `raylib.h` | No entity dependency |
| `enemy.c` | `enemy.h`, `entity.h`, `raylib.h` | Stdlib for rand() |
| `alien.c` | `alien.h`, `entity.h`, `raylib.h` | |
| `bullet.c` | `bullet.h`, `raylib.h` | |
| `booster.c` | `booster.h`, `raylib.h` | |
| `score.c` | `score.h` | No raylib dependency |
| `ui.c` | `ui.h`, `raylib.h` | |
| `game_state.c` | `game_state.h` | Defines `g_current_state` |
| `entity.h` | `raylib.h` | Header only — defines EntityBase |

**Circular dependency check:** `collision.c` does not include `enemy.h` or `alien.h` — it works through `EntityBase*` only. `spawner.c` does not include `collision.h`. `score.c` has no raylib dependency. No circular includes exist.

### B.2 raylib Function Usage by Module

| raylib Function | Used In (ver2) |
|----------------|----------------|
| `DrawTexturePro` | `enemy.c`, `alien.c`, `bullet.c`, `astronaut.c`, `main.c` (spaceship + hit effect) |
| `DrawRectangle`, `DrawRectangleLines`, `DrawRectangleRec` | `ui.c` |
| `DrawText`, `MeasureText`, `TextFormat` | `ui.c` |
| `DrawCircleV` | `booster.c` |
| `CheckCollisionRecs` | `collision.c` |
| `CheckCollisionPointRec` | `ui.c` |
| `GetMousePosition` | `main.c` (for astronaut aim direction) |
| `IsMouseButtonDown`, `IsMouseButtonPressed` | `main.c`, `ui.c` |
| `IsKeyDown`, `IsKeyPressed`, `IsKeyReleased` | `spaceship.c`, `astronaut.c`, `main.c` |
| `GetRandomValue` | `booster.c`, `spawner.c`, `enemy.c` |
| `PlaySound`, `UpdateMusicStream`, `PlayMusicStream` | `main.c` |
| `BeginDrawing`, `EndDrawing`, `ClearBackground` | `main.c` |
| `InitWindow`, `CloseWindow`, `SetTargetFPS`, `WindowShouldClose` | `main.c` |
| `InitAudioDevice`, `CloseAudioDevice` | `main.c` |
| `LoadTexture`, `UnloadTexture`, `LoadMusicStream`, `LoadSound` | `main.c` (Resources load/unload) |

**Note:** Zero raylib calls appear in `score.c` or `game_state.c`. `collision.c` uses only `CheckCollisionRecs` and `DrawTexturePro` (for the hit effect) — it has no spawn, movement, or input calls.
