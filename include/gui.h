#ifndef _GUI_H
#define _GUI_H

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <genesis/style_genesis.h>
#include <stddef.h>
#include <limits.h>

// struct for managing ui related vars
// and storing persistent data
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
	char *file_path;
} UiState;

// size used for relative values
#define MIN_SIZE 24
#define FONT_SIZE 16

// These functions render each of the components
// of the UI. Each of the functions return the height
// of the component to use in other components
int draw_psp_group(UiState *state, Rectangle pos);
int draw_ps_group(UiState *state, Rectangle pos);
int draw_timeline_group(UiState *state, Rectangle pos);
int draw_stats_group(UiState *state, Rectangle pos);
int draw_buttons(UiState *state, Rectangle pos);
int draw_main_window(UiState *state, Rectangle pos);
int draw_sub_window(UiState *state, Rectangle pos);

// These functions are responsible for the timeline
// rendering
void draw_process(Rectangle rect, const char *name, bool exited);
Texture generate_timeline(ProcessList exec_stack);

#ifdef GUI_IMPLEMENTATION

// Draws the Policy Specific Params group box
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
			if (GuiValueBox(
				value_rect,
				NULL,
				&(state->quantum),
				0,
				INT_MAX,
				state->quantum_edit
			)) state->quantum_edit = !state->quantum_edit;
			break;
		default:
			// Draws empty space if there is no options to render
			Rectangle space_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += space_rect.height + state->padding;
			GuiLabel(space_rect, NULL);
			break;
	}
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Policy Specifi Params");
	return pos.height;
}

// Draws the Policy Selector group box
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

// Draws the timeline in a scrollable panel
int draw_timeline_group(UiState *state, Rectangle pos) {
	const float header_height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + 2;
	Rectangle scroll_rect = {
		pos.x,
		pos.y,
		pos.width,
		state->timeline.height + header_height,
	};
	GuiScrollPanel(scroll_rect, "Timeline", (Rectangle){
		0, 0,
		state->timeline.width,
		state->timeline.height,
	}, &(state->panel_scroll), &(state->view_rect));
	BeginScissorMode(state->view_rect.x, state->view_rect.y, state->view_rect.width, state->view_rect.height);
	// Texture is y-flipped since Texture mode y-axis
	// is flipped compared to the usual Drawing mode
	DrawTexturePro(state->timeline, (Rectangle) {
			-state->panel_scroll.x,
			-state->panel_scroll.y,
			state->view_rect.width,
			-state->view_rect.height,
	}, state->view_rect, (Vector2){0, 0}, 0, WHITE);
	EndScissorMode();
	if (pos.height == 0) pos.height = scroll_rect.height;
	return pos.height;
}

// Draws the stats for generated timeline (avg. rotation time, etc...)
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

// Draws the generate and close buttons
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

// Draws the main window
int draw_main_window(UiState *state, Rectangle pos) {
	int y_increment = state->padding;
	Rectangle label_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += label_rect.height + state->padding;
	Rectangle ps_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	GuiLabel(label_rect, TextFormat("File selected: %s", state->file_path));
	ps_group_rect.height = draw_ps_group(state, ps_group_rect);

	y_increment += state->padding + ps_group_rect.height;
	Rectangle psp_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	psp_group_rect.height += draw_psp_group(state, psp_group_rect);
	y_increment += state->padding + psp_group_rect.height;

	Rectangle buttons_rect = {
		.x = pos.x + state->padding,
		.height = MIN_SIZE * 3,
		.width = pos.width - state->padding * 2,
	};
	buttons_rect.y = pos.height - state->padding - buttons_rect.height;
	draw_buttons(state, buttons_rect);
	y_increment += state->padding + buttons_rect.height;
	pos.height = y_increment + state->padding;
	return pos.height;
}

// Draws the results window
int draw_sub_window(UiState *state, Rectangle pos) {
	int y_increment = state->padding;
	Rectangle sub_anchor = {
		pos.x,
		pos.y + 32,
		pos.width,
		pos.height,
	};
	state->result = !GuiWindowBox(pos, "Scheduler Results");
	Rectangle timeline_group_rect = {
		sub_anchor.x + state->padding,
		sub_anchor.y + y_increment,
		sub_anchor.width - state->padding * 2,
	};
	y_increment += state->padding + draw_timeline_group(state, timeline_group_rect);
	Rectangle stats_group_rect = {
		sub_anchor.x + state->padding,
		sub_anchor.y + y_increment,
		sub_anchor.width - state->padding * 2,
	};
	y_increment += state->padding + draw_stats_group(state, stats_group_rect);
	pos.height = y_increment + state->padding;
	return pos.height;
}

void draw_process(Rectangle rect, const char *name, bool exited) {
	Color normal_color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
	Color exit_color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
	Color text_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	if (exited) DrawRectangleRec(rect, exit_color);
	else DrawRectangleRec(rect, normal_color);
	GuiDrawText(name, rect, TEXT_ALIGN_CENTER, text_color);
}

// This function will generate a texture of the gantt diagram
Texture generate_timeline(ProcessList exec_stack) {
	const int prev_color = GuiGetStyle(DEFAULT, LINE_COLOR);
	const int text_color = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	const int bg_color = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	GuiSetStyle(DEFAULT, LINE_COLOR, text_color);

	// Calculate texture dimensions
	const float block_size = MIN_SIZE * 2;
	const int padding = MIN_SIZE;
	const float width = exec_stack.count * block_size + padding * 2;
	const float height = block_size + padding * 2;

	RenderTexture timeline = LoadRenderTexture(width, height);
	BeginTextureMode(timeline);
	ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

	for (int i = 0; i < exec_stack.count; i++) {
		Process p = exec_stack.list[i];
		draw_process((Rectangle){
			padding + block_size * i,
			padding,
			block_size,
			block_size,
		}, p.name, (bool) p.remaining_time <= 0);
	}
	GuiGrid((Rectangle){
		padding,
		padding,
		exec_stack.count * block_size,
		block_size,
	}, NULL, block_size, 1, NULL);
	EndTextureMode();
	GuiSetStyle(DEFAULT, LINE_COLOR, prev_color);
	return timeline.texture;
}

#endif // GUI_IMPLEMENTATION
#endif // _GUI_H
