#include "parser.h"
#include "process.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GUI_IMPLEMENTATION
#include <gui.h>

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
	UiState state = {.padding = MIN_SIZE / 2, .file_path = file_path};
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

		draw_main_window(&state, main_anchor);
		if (state.result) {
			// TODO: Add button for saving the texture as an image
			if (!state.generated) {
				state.timeline = generate_timeline(execution_stack);
				state.generated = true;
			}
			draw_sub_window(&state, main_anchor);
		}
		EndDrawing();
	}

	// GUI Cleanup
	CloseWindow();
  free(plist.list);
  return 0;
}
