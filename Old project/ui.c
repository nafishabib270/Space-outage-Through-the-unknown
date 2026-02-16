#include "ui.h"

bool button(Rectangle bounds, const char* text) {
	Vector2 mouse = GetMousePosition();

	bool hover = CheckCollisionPointRec(mouse, bounds);

	DrawRectangleRec(bounds, hover ? DARKGRAY : GRAY);
	DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, BLACK);
	DrawText(text, bounds.x + 20, bounds.y + 10, 30, BLACK);

	return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

}