#include "headers/all.h"
#include <math.h>

/*

 Game Ideas:
- Turn-based
- Maybe multiplayer
- "War Groove"-kinda style
- "pique bandeirinha"

*/

uint scene_main(void) {
	mat4 object;
	
	// Camera
	struct Camera camera = {
		.pos = { 0 },
		.angle = 0,
		.zoom = 2.0f / SCREEN_SCALE,
		.speed = 0.2f
	};
	
	// Tilemap
	u16 tilemapData[8*8] = {
		0, 1, 1, 1, 1, 1, 1, 0,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 0, 1, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 1, 1, 1, 0, 1,
		1, 0, 0, 1, 1, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 1, 1, 1, 1, 1, 0,
	};
	
	struct Tilemap tilemap = {
		.texture = &assets_textures[TEX_TILE_DEFAULT],
		.size = { 8, 8 },
		.data = tilemapData
	};
	
	// Selected Block
	Sprite spriteSelected = {
		.texture = &assets_textures[TEX_SPRITES_0],
		.size = { 16, 16 },
		.offset = { 16, 0 }
	};
	
	// Game Loop
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		//- Update
		if (gamepad_is_pressed(GPAD_BUTTON_START))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		camera.targetPos[0] += (keyboard_is_down(GLFW_KEY_D) - keyboard_is_down(GLFW_KEY_A)) * 3.0f * game.deltaTime;
		camera.targetPos[1] += (keyboard_is_down(GLFW_KEY_S) - keyboard_is_down(GLFW_KEY_W)) * 3.0f * game.deltaTime;
		
		camera_update(&camera);
		
		//- Draw
		glClearColor(0.0f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		mat4 view;
		camera_matrix(&camera, view);
		
		// Draw Tilemap
		glm_mat4_identity(object);
		glm_scale(object, (vec3) { 2, 2 });
		glm_translate(object, (vec3) { -(f32)tilemap.size[0] / 2.0f * tilemap.texture->size[0], -(f32)tilemap.size[1] / 2.0f * tilemap.texture->size[1] });
		glm_mat4_mul(view, object, object);
		tilemap_render(&tilemap, object);
		
		// Draw Selected Tile
		vec2 mousePos;
		camera_mouse_pos(&camera, mousePos);
		
		f32 tilewidth = tilemap.texture->size[0] * 2.0f;
		f32 tileheight = tilemap.texture->size[1] * 2.0f;
		
		mousePos[0] = floorf(mousePos[0] / tilewidth) * tilewidth + tilewidth / 2;
		mousePos[1] = floorf(mousePos[1] / tileheight) * tileheight + tileheight / 2;
		
		f32 scale = 2.2f + sinf(game.frameCount * 0.2f) * 0.1f;
		
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { mousePos[0], mousePos[1] });
		glm_scale(object, (vec3) { scale, scale });
		glm_mat4_mul(view, object, object);
		sprite_batch_add(NULL, &spriteSelected, object, 0xFFFFFFFF, 0, ALIGNMENT_MIDDLE | ALIGNMENT_CENTER);
		
		//- Finish Frame
		engine_end_frame();
	}
	
	// Close the game
	game.currentScene = NULL;
	
	return 0;
}
