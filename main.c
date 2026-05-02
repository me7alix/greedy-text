#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
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

int EncodeCodepointUtf8(int cp, char *out) {
	if (cp <= 0x7F) {
		out[0] = cp;
		return 1;
	} else if (cp <= 0x7FF) {
		out[0] = 0xC0 | (cp >> 6);
		out[1] = 0x80 | (cp & 0x3F);
		return 2;
	} else if (cp <= 0xFFFF) {
		out[0] = 0xE0 | (cp >> 12);
		out[1] = 0x80 | ((cp >> 6) & 0x3F);
		out[2] = 0x80 | (cp & 0x3F);
		return 3;
	} else {
		out[0] = 0xF0 | (cp >> 18);
		out[1] = 0x80 | ((cp >> 12) & 0x3F);
		out[2] = 0x80 | ((cp >> 6) & 0x3F);
		out[3] = 0x80 | (cp & 0x3F);
		return 4;
	}
}

int main(void) {
	SetConfigFlags(
		FLAG_WINDOW_RESIZABLE |
		FLAG_VSYNC_HINT);

	InitWindow(800, 800, "Earth");

	int count;
	const char *text = LoadFileText("earth.txt");
	int *codepoints = LoadCodepoints(text, &count);
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

		for (int idx = 0; idx < count; idx++) {
			char ch_str[5];
			int ch_len = EncodeCodepointUtf8(codepoints[idx], ch_str);
			ch_str[ch_len] = '\0';

			Vector2 size = MeasureTextEx(font, ch_str, FONT_SIZE, 0);

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

			DrawTextEx(font, ch_str, pos, FONT_SIZE, 0, WHITE);
			pos.x += size.x + FONT_SPACING;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
