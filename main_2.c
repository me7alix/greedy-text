#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#define FONT_SIZE 20
#define MAX_SPACING 15
#define DELIMS " -\n"
#define STEP 2.0f

typedef struct {
	Vector2 start, end;
	char words[64][128];
	float words_width;
	size_t count;
} Line;

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
	float space_width = MeasureTextEx(font, " ", FONT_SIZE, 0).x;

	Vector2 cir_pos = {
		GetScreenWidth() / 2,
		GetScreenHeight() / 2
	};

	while (!WindowShouldClose()) {	
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			cir_pos = GetMousePosition();
		}

		static Line lines[256];
		memset(lines, 0, sizeof(lines));
		size_t lcnt = 0;

		Vector2 pos = {0};

		char buf[1 << 18];
		memcpy(buf, text, text_len);
		char *tok = strtok(buf, DELIMS);

		while (tok != NULL) {
			Vector2 size = MeasureTextEx(font, tok, FONT_SIZE, 0);

			if (pos.x + size.x > GetScreenWidth()) {
				lines[lcnt++].end = (Vector2){
					GetScreenWidth(), pos.y
				};

				pos.x = 0;
				pos.y += FONT_SIZE;
			}

			bool newLine = false;
			while (RectCircleIntersect(pos, size, cir_pos, cir_rad)) {
				if (!newLine) {
					newLine = true;
					pos.x -= size.x + MAX_SPACING;
					while (!RectCircleIntersect(pos, size, cir_pos, cir_rad))
						pos.x += STEP;
					lines[lcnt++].end = Vector2Add(pos, size);
				}

				if (pos.x + size.x > GetScreenWidth()) {
					lines[lcnt++].end = (Vector2){
						GetScreenWidth(),
						pos.y
					};

					pos.x = 0;
					pos.y += FONT_SIZE;
				}

				pos.x += STEP;
			}

			pos.x -= STEP;

			if (lines[lcnt].count == 0) {
				lines[lcnt].start = pos;
			}

			lines[lcnt].words_width += size.x;

			memcpy(lines[lcnt].words[lines[lcnt].count++], tok, strlen(tok)+1);
			pos.x += size.x + MAX_SPACING;
			tok = strtok(NULL, DELIMS);
		}

		/* lcnt++;
		 *
		 * BUG: Last line looks horrible and
		 * I don't know how it fix it so for
		 * now I just don't include it
		*/

		BeginDrawing();
		ClearBackground(BLACK);

		DrawTexture(cosmos, 0, 0, WHITE);
		DrawTexture(earth, cir_pos.x - cir_rad, cir_pos.y - cir_rad, WHITE);

		for (size_t i = 0; i < lcnt; i++) {
			float x = lines[i].start.x;
			float spacing = ((lines[i].end.x - lines[i].start.x) - lines[i].words_width) /
				                         (lines[i].count - 1);

			for (size_t j = 0; j < lines[i].count; j++) {	
				Vector2 pos = {x, lines[i].start.y};
				Vector2 size = MeasureTextEx(font, lines[i].words[j], FONT_SIZE, 0);
				DrawTextEx(font, lines[i].words[j], pos, FONT_SIZE, 0, WHITE);
				x += spacing + size.x;
			}
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
