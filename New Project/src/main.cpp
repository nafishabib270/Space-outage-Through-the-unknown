#include "GameLoop.h"

/*
 * main.cpp
 * --------
 * The entry point contains no logic whatsoever. Its only job is to
 * construct the GameLoop (which is the composition root) and call run().
 *
 * Compare with the original main.c, which contained:
 *   - 30 global variables
 *   - DrawHealthBar() definition
 *   - ResetGame() definition
 *   - All collision loops
 *   - All spawn timing
 *   - All input handling
 *   - All rendering
 *   ... in a single 550-line function.
 *
 * The 550-line main() is now these 5 lines.
 */

int main() {
    GameLoop game;
    game.run();
    return 0;
}
