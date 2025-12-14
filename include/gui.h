#ifndef _GUI_H
#define _GUI_H

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <genesis/style_genesis.h>
#include <stddef.h>
#include <limits.h>
#undef RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include <custom_file_dialog/gui_window_file_dialog.h>
#include <scheduler.h>
#include <dynamic_loader.h>

typedef struct {
	bool quit;
	bool result;
	bool generated;
	int selected_policy;
	float padding;
	int quantum;
	bool quantum_edit;
	bool aging_edit;
	Vector2 panel_scroll;
	int scroll_index;
	int list_selected;
	Rectangle view_rect;
	GuiWindowFileDialogState file_dialog_state;

	ProcessList plist;
	ProcessList exec_stack;
	float avg_rot;
	float avg_wait;
	Texture timeline;
	char file_path[512];
	char error_msg[128];
	int aging;
} UiState;

#define MIN_SIZE 24
#define FONT_SIZE 16

UiState init_state();

float draw_main_window(UiState *state, Rectangle pos);
float draw_sub_window(UiState *state, Rectangle pos);

Texture generate_timeline(ProcessList exec_stack);

#ifdef GUI_IMPLEMENTATION

UiState init_state() {
	return (UiState) {
		.file_dialog_state = InitGuiWindowFileDialog(GetWorkingDirectory()),
		.list_selected = -1,
		.error_msg[0] = '\0',
	};
}

static float draw_pv_group(UiState *state, Rectangle pos) {
	char *elems;
	for (int i=0; i < state->plist.count; i++) {
		if (i > 0) elems = TextFormat("%s;%s", elems, state->plist.list[i].name);
		else elems = TextFormat("%s", state->plist.list[i].name);
	}

	float y_increment = state->padding;
	float x_increment = state->padding;
	Rectangle list_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		(pos.width - state->padding * 2) * 0.3,
		MIN_SIZE * 6,
	};
	x_increment += list_rect.width + state->padding;

	Rectangle label_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		(pos.width - state->padding * 2) * 0.3,
		MIN_SIZE,
	};
	y_increment += label_rect.height + state->padding;

	Rectangle label2_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		(pos.width - state->padding * 2) * 0.3,
		MIN_SIZE,
	};
	y_increment += label2_rect.height + state->padding;

	Rectangle label3_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		(pos.width - state->padding * 2) * 0.3,
		MIN_SIZE,
	};
	y_increment += label3_rect.height + state->padding;

	x_increment += label_rect.width + state->padding;
	y_increment = fmax(
		list_rect.height + state->padding * 2,
		y_increment
	);

	GuiListView(list_rect, elems, &(state->scroll_index), &(state->list_selected));
	if (state->list_selected >= 0) {
		GuiLabel(label_rect, TextFormat("Arrival: %d", state->plist.list[state->list_selected].arrival_time));
		GuiLabel(label2_rect, TextFormat("Burst: %d", state->plist.list[state->list_selected].burst_time));
		GuiLabel(label3_rect, TextFormat("Priority: %d", state->plist.list[state->list_selected].priority));
	}
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Process Viewer");
	return pos.height;
}

static float draw_psp_group(UiState *state, Rectangle pos) {
	float y_increment = state->padding;
	switch (state->selected_policy) {
		case 3:
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
				1,
				INT_MAX,
				state->quantum_edit
			)) state->quantum_edit = !state->quantum_edit;
			break;
		case 4:
			Rectangle label1_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += label1_rect.height + state->padding;
			Rectangle value1_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += value1_rect.height + state->padding;

			Rectangle label2_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += label2_rect.height + state->padding;
			Rectangle value2_rect = {
				pos.x + state->padding,
				pos.y + y_increment,
				pos.width - state->padding * 2,
				MIN_SIZE,
			};
			y_increment += value2_rect.height + state->padding;

			GuiLabel(label1_rect, "Quantum");
			if (GuiValueBox(
				value1_rect,
				NULL,
				&(state->quantum),
				1,
				INT_MAX,
				state->quantum_edit
			)) state->quantum_edit = !state->quantum_edit;
			
			GuiLabel(label2_rect, "Aging");
			if (GuiValueBox(
				value2_rect,
				NULL,
				&(state->aging),
				5,
				INT_MAX,
				state->aging_edit
			)) state->aging_edit = !state->aging_edit;

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
	GuiGroupBox(pos, "Policy Specific Params");
	return pos.height;
}

static float draw_ps_group(UiState *state, Rectangle pos) {
	float y_increment = state->padding;
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

	char **loaded_policies;
	int count = load_policies("policies", &loaded_policies);
	int total = sp_length + count;

	char **all_policies = malloc(sizeof(char*) * total);

	for (int i = 0; i < sp_length; i++) {
		all_policies[i] = supported_policies[i];
	}

	for (int i = 0; i < count; i++) {
    	all_policies[sp_length + i] = loaded_policies[i];
	}

	GuiComboBox(combobox_rect, TextJoin(
		all_policies,
		total,
		";"
	), &(state->selected_policy));
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Policy Selector");
	return pos.height;
}

static float draw_timeline_group(UiState *state, Rectangle pos) {
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

static float draw_stats_group(UiState *state, Rectangle pos) {
	float y_increment = state->padding;
	Rectangle rot_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += state->padding + rot_rect.height;

	Rectangle wait_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += state->padding + rot_rect.height;
	GuiLabel(rot_rect, TextFormat("Avg. Rotation time: %f", state->avg_rot));
	GuiLabel(wait_rect, TextFormat("Avg. Wait time: %f", state->avg_wait));
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "Stats");
	return pos.height;
}

static float draw_buttons(UiState *state, Rectangle pos) {
	float x_increment = state->padding;
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

static float draw_file_group(UiState *state, Rectangle pos) {
	float y_increment = state->padding;
	float x_increment = state->padding;
	const char *button_text = "Select File";
	float button_width = fmax(
		MIN_SIZE * 5,
		MeasureText(button_text, GuiGetStyle(DEFAULT, TEXT_SIZE))
	);

	Rectangle label_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		pos.width - state->padding * 3 - button_width,
		MIN_SIZE,
	};
	x_increment += label_rect.width + state->padding;

	Rectangle button_rect = {
		pos.x + x_increment,
		pos.y + y_increment,
		button_width,
		MIN_SIZE,
	};
	y_increment += label_rect.height + state->padding;

	Rectangle label2_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
		MIN_SIZE,
	};
	y_increment += label2_rect.height + state->padding;

	GuiLabel(label_rect, TextFormat("File selected: %s", state->file_path));
	if (GuiButton(button_rect, button_text)) state->file_dialog_state.windowActive = true;
	GuiLabel(label2_rect, TextFormat("Loaded %d processes", state->plist.count));
	if (pos.height == 0) pos.height = y_increment;
	GuiGroupBox(pos, "File Picker");
	return y_increment;
}

float draw_main_window(UiState *state, Rectangle pos) {
	float y_increment = state->padding;

	Rectangle file_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	file_group_rect.height = draw_file_group(state, file_group_rect);
	y_increment += file_group_rect.height + state->padding;

	Rectangle ps_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	ps_group_rect.height = draw_ps_group(state, ps_group_rect);
	y_increment += state->padding + ps_group_rect.height;

	Rectangle psp_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	psp_group_rect.height += draw_psp_group(state, psp_group_rect);
	y_increment += state->padding + psp_group_rect.height;

	Rectangle pv_group_rect = {
		pos.x + state->padding,
		pos.y + y_increment,
		pos.width - state->padding * 2,
	};
	pv_group_rect.height += draw_pv_group(state, pv_group_rect);
	y_increment += state->padding + pv_group_rect.height;

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

float draw_sub_window(UiState *state, Rectangle pos) {
	float y_increment = state->padding;
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

static void draw_process(Rectangle rect, const char *name, bool exited) {
	Color normal_color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL));
	Color exit_color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
	Color text_color = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
	if (exited) DrawRectangleRec(rect, exit_color);
	else DrawRectangleRec(rect, normal_color);
	GuiDrawText(name, rect, TEXT_ALIGN_CENTER, text_color);
}

Texture generate_timeline(ProcessList exec_stack) {
	const int prev_color = GuiGetStyle(DEFAULT, LINE_COLOR);
	const int text_color = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	const int bg_color = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);
	GuiSetStyle(DEFAULT, LINE_COLOR, text_color);

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

#endif
#endif
