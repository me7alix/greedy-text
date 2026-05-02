#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <math.h>

#define FONT_SIZE    18
#define FONT_SPACING 3

float sdBox2D(Vector2 p, Vector2 b) {
	Vector2 d = Vector2Subtract((Vector2){ fabsf(p.x), fabsf(p.y) }, b);
	Vector2 dmax = { fmaxf(d.x, 0.0f), fmaxf(d.y, 0.0f) };
	float outside = Vector2Length(dmax);
	float inside = fminf(fmaxf(d.x, d.y), 0.0f);
	return outside + inside;
}

bool RectCircleIntersect(Vector2 rectPos, Vector2 rectSize, Vector2 circlePos, float circleRad) {
    Vector2 rectCenter = {
        rectPos.x + rectSize.x * 0.5f,
        rectPos.y + rectSize.y * 0.5f
    };

    Vector2 halfSize = {
        rectSize.x * 0.5f,
        rectSize.y * 0.5f
    };

    Vector2 p = Vector2Subtract(circlePos, rectCenter);
    float dist = sdBox2D(p, halfSize);

    return dist < circleRad;
}

int main(void) {
    SetConfigFlags(
		FLAG_WINDOW_RESIZABLE |
		FLAG_VSYNC_HINT);
    InitWindow(800, 800, "Earth");

    Font font = GetFontDefault();
    Texture2D cosmos = LoadTexture("cosmos.png");
    Texture2D earth = LoadTexture("earth.png");
	const char *text = LoadFileText("earth.txt");
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

        for (int idx = 0; text[idx] != '\0'; idx++) {
            char ch_str[2] = { text[idx], '\0' };
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
