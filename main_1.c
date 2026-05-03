#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#define FONT_SIZE 20
#define FONT_SPACING 3

float sdBox2D(Vector2 p, Vector2 b) {
	Vector2 d = Vector2Subtract((Vector2){ fabsf(p.x), fabsf(p.y) }, b);
	Vector2 dmax = { fmaxf(d.x, 0.0f), fmaxf(d.y, 0.0f) };
	float outside = Vector2Length(dmax);
	float inside = fminf(fmaxf(d.x, d.y), 0.0f);
	return outside + inside;
}

bool RectCircleIntersect(Vector2 rp, Vector2 rs, Vector2 cp, float cr) {
	Vector2 rectCenter = {
		rp.x + rs.x * 0.5f,
		rp.y + rs.y * 0.5f
	};

	Vector2 halfSize = {
		rs.x * 0.5f,
		rs.y * 0.5f
	};

	Vector2 p = Vector2Subtract(cp, rectCenter);
	float dist = sdBox2D(p, halfSize);
	return dist < cr;
}

int main(void) {
	SetConfigFlags(
		FLAG_WINDOW_RESIZABLE |
		FLAG_VSYNC_HINT);

	InitWindow(800, 800, "Earth");

	int count;
	char *text = LoadFileText("earth.txt");
	size_t text_len = strlen(text);
	Font font = LoadFont("Quivira.otf");
	Texture2D cosmos = LoadTexture("cosmos.png");
	Texture2D earth = LoadTexture("earth.png");	
	float cir_rad = earth.width / 2;

	Vector2 cir_pos = {
		GetScreenWidth() / 2,
		GetScreenHeight() / 2
	};

	while (!WindowShouldClose()) {
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			cir_pos = GetMousePosition();
		}

		BeginDrawing();
		ClearBackground(BLACK);

		DrawTexture(cosmos, 0, 0, WHITE);
		DrawTexture(earth, cir_pos.x - cir_rad, cir_pos.y - cir_rad, WHITE);

		Vector2 pos = {0};
		char buf[1 << 18];
		memcpy(buf, text, text_len);
		char *tok = strtok(buf, " -\n");

		while (tok != NULL) {
			Vector2 size = MeasureTextEx(font, tok, FONT_SIZE, 0);

			if (pos.x + size.x > GetScreenWidth()) {
				pos.x = 0;
				pos.y += FONT_SIZE;
			}

			while (RectCircleIntersect(pos, size, cir_pos, cir_rad)) {
				pos.x += 4.0f;
				if (pos.x + size.x > GetScreenWidth()) {
					pos.x = 0;
					pos.y += FONT_SIZE;
				}
			}

			DrawTextEx(font, tok, pos, FONT_SIZE, 0, WHITE);
			pos.x += size.x + FONT_SPACING;
			tok = strtok(NULL, " -\n");
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
