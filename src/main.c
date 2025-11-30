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
	int selected_policy;
	int padding;

	int quantum;
	bool quantum_edit;
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
	if (GuiButton(gen_rect, "Generate")) state->result = true;
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
	int screen_height = 312;
	UiState state = {.padding = MIN_SIZE / 2};
	Rectangle main_anchor = {0, 0, screen_width, screen_height};

	// GUI Initialization
	InitWindow(screen_width, screen_height, "Scheduler");
	SetTargetFPS(60);
	GuiLoadStyleGenesis();

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
			state.result = !GuiWindowBox((Rectangle){
					main_anchor.x,
					main_anchor.y,
					main_anchor.width / 2,
					main_anchor.height / 2,
			}, "Scheduler Results");
		}
		EndDrawing();
	}

	// GUI Cleanup
	CloseWindow();
  free(plist.list);
  return 0;
}
