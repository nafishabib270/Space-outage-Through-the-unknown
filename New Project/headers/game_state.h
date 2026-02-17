#ifndef GAME_STATE_H
#define GAME_STATE_H

typedef enum Game_state {
	MENU,
	GAME,
	EXIT
}GameState;

extern GameState current_state;

#endif
