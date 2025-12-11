#include "parser.h"
#include "process.h"
#include "scheduler.h"
#include <stats.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GUI_IMPLEMENTATION
#include <gui.h>

// TODO: Add button for saving the texture as an image

void Usage(char *prg) {
	printf("USAGE: %s -i=config_file\n", prg);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	if (argc < 2) Usage(argv[0]);

	// GUI Related Variables
	Args args = parse_args(argc, argv);

	int screen_width = 312 * 3;
	int screen_height = 312 * 2;
	Rectangle main_anchor = {0, 0, screen_width, screen_height};
	UiState state = init_state();
	if (args.file_path != NULL) {
		strcpy(state.file_path, args.file_path);
		state.plist = parse_file(state.file_path);
	}
	state.padding = MIN_SIZE / 2;
	state.file_dialog_state.windowBounds = main_anchor;

	// GUI Initialization
	InitWindow(screen_width, screen_height, "Scheduler");
	SetTargetFPS(60);
	GuiLoadStyleGenesis();
	GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);

	while (!state.quit)
	{
		state.quit = WindowShouldClose();

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		if (state.file_dialog_state.windowActive) GuiLock();
		if (state.file_dialog_state.SelectFilePressed) {
			strcpy(state.file_path, TextFormat(
				"%s" PATH_SEPERATOR "%s",
				state.file_dialog_state.dirPathText,
				state.file_dialog_state.fileNameText
			));
			free(state.plist.list);
			state.plist = parse_file(state.file_path);
			state.file_dialog_state.SelectFilePressed = false;
		}
		draw_main_window(&state, main_anchor);
		if (state.result) {
			if (!state.generated) {
				if (state.exec_stack.list != NULL) {
					free(state.exec_stack.list);
					state.exec_stack.list = NULL;
				}
				if (state.selected_policy >= sp_length) {
					snprintf(state.error_msg, sizeof(state.error_msg),
							"Scheduler error: unknown policy");
					state.avg_rot = 0;
					state.avg_wait = 0;
					state.timeline = (Texture2D){0};
					state.generated = false;
				} else {
					state.error_msg[0] = '\0';
					state.exec_stack = scheduler(
					state.plist,
					supported_policies[state.selected_policy],
					(Params) {
						.quantum = state.quantum
					}
					);
					state.avg_rot = calculate_avg_rot(state.exec_stack);
					state.avg_wait = calculate_avg_wait(state.exec_stack);
					state.timeline = generate_timeline(state.exec_stack);
					state.generated = true;
				}
			}
			
			draw_sub_window(&state, main_anchor);
			if (state.error_msg[0] != '\0') {
				DrawText(
					state.error_msg,
					20,
					screen_height - 30,
					20,
					RED
				);
			}
			
		}
		GuiUnlock();
		GuiWindowFileDialog(&(state.file_dialog_state));
		EndDrawing();
	}

	// GUI Cleanup
	UnloadTexture(state.timeline);
	CloseWindow();
  free(state.plist.list);
  return 0;
}
