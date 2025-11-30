#include "parser.h"
#include "process.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <genesis/style_genesis.h>
#include <limits.h>

// TODO: Organize code and document it properly

typedef struct {
	bool quit;
	bool result;
	bool generated;
	int selected_policy;
	int padding;
	int quantum;
	bool quantum_edit;
	Vector2 panel_scroll;
	Rectangle view_rect;

	float avg_rot;
	Texture timeline;
} UiState;

#define MIN_SIZE 24

int draw_psp_group(UiState *state, Rectangle pos) {
	int y_increment = state->padding;
	switch (state->selected_policy) {
		case 1:
			Rectangle label_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += label_rect.height + state->padding;
			Rectangle value_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += value_rect.height + state->padding;
			GuiLabel(label_rect, "Quantum");
			if (GuiValueBox(value_rect, NULL, &(state->quantum), 0, INT_MAX, state->quantum_edit)) state->quantum_edit = !state->quantum_edit;
			break;
	}
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Policy Specifi Params");
	return pos.height;
}

int draw_ps_group(UiState *state, Rectangle pos) {
	int y_increment = state->padding;
	Rectangle label_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += label_rect.height + state->padding;
	Rectangle combobox_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += combobox_rect.height + state->padding;
	GuiLabel(label_rect, "Choose policy");
	GuiComboBox(combobox_rect, "FIFO;Round-Robin;Preemptive Priority", &(state->selected_policy));
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Policy Selector");
	return pos.height;
}

int draw_timeline_group(UiState *state, Rectangle pos) {
	Rectangle scroll_rect = {
		pos.x,
		pos.y,
		pos.width,
		fmin(state->timeline.height, MIN_SIZE * 10),
	};
	GuiScrollPanel(scroll_rect, "Timeline", (Rectangle){
		0, 0,
		state->timeline.width,
		state->timeline.height,
	}, &(state->panel_scroll), &(state->view_rect));
	BeginScissorMode(state->view_rect.x, state->view_rect.y, state->view_rect.width, state->view_rect.height);
	DrawTexturePro(state->timeline, (Rectangle) {
			-state->panel_scroll.x,
			-state->panel_scroll.y,
			state->view_rect.width,
			state->view_rect.height,
	}, state->view_rect, (Vector2){0, 0}, 0, WHITE);
	EndScissorMode();
	if (pos.height == 0) pos.height = scroll_rect.height;
	return pos.height;
}

int draw_stats_group(UiState *state, Rectangle pos) {
	int y_increment = state->padding;
	Rectangle rot_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += state->padding + rot_rect.height;
	GuiLabel(rot_rect, TextFormat("Avg. Rotation time: %f", state->avg_rot));
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Stats");
	return pos.height;
}

// This function will generate a texture of the gantt diagram
// TODO: Impelement correct rendering
Texture generate_timeline(ProcessList exec_stack) {
	RenderTexture timeline = LoadRenderTexture(1280, 720);
	BeginTextureMode(timeline);
	ClearBackground(SKYBLUE);
	DrawRectangle(MIN_SIZE, timeline.texture.height - MIN_SIZE * 2, 50, MIN_SIZE, GREEN);
	GuiGrid((Rectangle){
		MIN_SIZE,
		timeline.texture.height - MIN_SIZE * 2,
		10 * MIN_SIZE * 2,
		MIN_SIZE,
	}, NULL, MIN_SIZE, 1, NULL);
	EndTextureMode();
	return timeline.texture;
}

int draw_buttons(UiState *state, Rectangle pos) {
	int x_increment = state->padding;
	Rectangle close_rect = {
		pos.x + x_increment,
		pos.y + state->padding,
		MIN_SIZE * 3,
		pos.height - state->padding * 2,
	};
	x_increment += close_rect.width + state->padding;
	Rectangle gen_rect = {
		pos.x + x_increment,
		pos.y + state->padding,
		fmax(MIN_SIZE * 3, MeasureText("Generate", GuiGetStyle(DEFAULT, TEXT_SIZE))),
		pos.height - state->padding * 2,
	};
	if (GuiButton(close_rect, "Close")) state->quit = true;
	if (GuiButton(gen_rect, "Generate")) {
		state->generated = false;
		state->result = true;
	}
	return 0;
}

int main(void) {
  ProcessList plist;
	char *file_path = "./config/processes.txt";
  plist = parse_file(file_path);
  for (int i = 0; i < plist.count; i++) {
    printf("%s %d %d %d\n", plist.list[i].name, plist.list[i].arrival_time,
           plist.list[i].burst_time, plist.list[i].priority);
  }

  const char *policy_name = "preemptive_priority";

  ProcessList execution_stack = scheduler(plist, policy_name);
  for (int i = 0; i < execution_stack.count; i++) {
    printf("%s\n", execution_stack.list[i].name);
  }

	// GUI Related Variables
	int screen_width = 312;
	int screen_height = 312 * 1.5;
	UiState state = {.padding = MIN_SIZE / 2};
	Rectangle main_anchor = {0, 0, screen_width, screen_height};

	// GUI Initialization
	InitWindow(screen_width, screen_height, "Scheduler");
	SetTargetFPS(60);
	GuiLoadStyleGenesis();
	GuiSetStyle(DEFAULT, TEXT_SIZE, 16);

	while (!state.quit)
	{
		state.quit = WindowShouldClose();

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		int y_increment = state.padding;
		Rectangle label_rect = {
			main_anchor.x + state.padding,
			main_anchor.y + y_increment,
			main_anchor.width - state.padding * 2,
			MIN_SIZE,
		};
		y_increment += label_rect.height + state.padding;
		Rectangle ps_group_rect = {
			main_anchor.x + state.padding,
			main_anchor.y + y_increment,
			main_anchor.width - state.padding * 2,
		};
		GuiLabel(label_rect, TextFormat("File selected: %s", file_path));
		ps_group_rect.height = draw_ps_group(&state, ps_group_rect);

		y_increment += state.padding + ps_group_rect.height;
		Rectangle psp_group_rect = {
			main_anchor.x + state.padding,
			main_anchor.y + y_increment,
			main_anchor.width - state.padding * 2,
		};
		psp_group_rect.height += draw_psp_group(&state, psp_group_rect);
		y_increment += state.padding + psp_group_rect.height;

		Rectangle buttons_rect = {
			.x = main_anchor.x + state.padding,
			.height = MIN_SIZE * 3,
			.width = main_anchor.width - state.padding * 2,
		};
		buttons_rect.y = main_anchor.height - state.padding - buttons_rect.height;
		draw_buttons(&state, buttons_rect);

		if (state.result) {
			// TODO: Add button for saving the texture as an image
			if (!state.generated) {
				state.timeline = generate_timeline(execution_stack);
				state.generated = true;
			}
			Rectangle sub_anchor = {
				main_anchor.x,
				main_anchor.y + 32,
				main_anchor.width,
				main_anchor.height,
			};
			state.result = !GuiWindowBox(main_anchor, "Scheduler Results");

			int y_increment = state.padding;
			Rectangle timeline_group_rect = {
				sub_anchor.x + state.padding,
				sub_anchor.y + y_increment,
				sub_anchor.width - state.padding * 2,
			};
			y_increment += state.padding + draw_timeline_group(&state, timeline_group_rect);
			Rectangle stats_group_rect = {
				sub_anchor.x + state.padding,
				sub_anchor.y + y_increment,
				sub_anchor.width - state.padding * 2,
			};
			y_increment += state.padding + draw_stats_group(&state, stats_group_rect);
		}
		EndDrawing();
	}

	// GUI Cleanup
	CloseWindow();
  free(plist.list);
  return 0;
}
