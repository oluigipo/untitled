#include "headers/all.h"

func void scene_menu_draw_button(const vec4 rect, vec4 mousePos, string text, mat4 view, Texture* font, b32 disabled) {
	mat4 matrix;
	
	// Rectangle
	glm_mat4_identity(matrix);
	glm_translate(matrix, (vec3) { rect[0], rect[1] });
	glm_scale(matrix, (vec3) { rect[2], rect[3] });
	glm_mat4_mul(view, matrix, matrix);
	u32 color;
	
	if (disabled) color = 0xFF080808;
	else color = collision_aabb(mousePos, rect) ? 0xFF222222 : 0xFF0f0f0f;
	
	primitive_render_roundrect(matrix, color, PI32 / 3, (vec2) { 1.0f, 0.25f });
	
	// Text
	glm_mat4_identity(matrix);
	glm_translate(matrix, (vec3) { rect[0] + rect[2] / 2, rect[1] + rect[3] / 2 });
	glm_scale(matrix, (vec3) { 1.5f, 1.5f });
	glm_mat4_mul(view, matrix, matrix);
	
	uint cc[2] = {
		0,
		(disabled) ? 0xFF333333 : 0xFFFFFFFF
	};
	
	text_render_ext(text, matrix, font, cc, ALIGNMENT_CENTER | ALIGNMENT_MIDDLE);
}

func uint scene_menu(void) {
	Texture* font = &assets_textures[TEX_DEFAULT_FONT];
	
	struct Camera camera = {
		.zoom = 1.0f
	};
	
	f32 ypos = 0.0f;
	vec4 buttonCreate = {
		-(f32)game.window.width * 0.4, ypos,
		200.0f, 50.0f
	};
	ypos += 75.0f;
	vec4 buttonPlay = {
		-(f32)game.window.width * 0.4, ypos,
		200.0f, 50.0f
	};
	ypos += 75.0f;
	vec4 buttonExit = {
		-(f32)game.window.width * 0.4, ypos,
		200.0f, 50.0f
	};
	
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		vec4 mousePos;
		{
			vec2 mouse_2;
			camera_mouse_pos(&camera, mouse_2);
			
			mousePos[0] = mouse_2[0];
			mousePos[1] = mouse_2[1];
			mousePos[2] = mousePos[3] = 0.0f;
		}
		
		//- Update
		if (keyboard_is_pressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		if (mouse.button[0] && !mouse.oldButton[0]) {
			
			// Create or exit lobby
			if (collision_aabb(mousePos, buttonCreate) && !discord.joining) {
				
				if (discord_is_connected_to_lobby()) discord_exit_lobby();
				else discord_create_lobby();
				
			}
			
			// Play
			else if (collision_aabb(mousePos, buttonPlay) && discord_is_connected_to_lobby()) {
				os_message_box("Aviso!", "não dá pra jogar ainda :peen");
			}
			
			// Exit game
			else if (collision_aabb(mousePos, buttonExit)) {
				glfwSetWindowShouldClose(game.apiWindow, true);
			}
			
		}
		
		//- Draw
		mat4 view;
		camera_matrix(&camera, view);
		
		mat4 matrix;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw Title
		{
			glm_mat4_identity(matrix);
			glm_translate(matrix, (vec3) { 0, -(f32)game.window.height * 0.40f });
			f32 scale = sinf(glfwGetTime() * 0.5f) * 0.25f + 3.5f;
			glm_scale(matrix, (vec3) { scale, scale });
			glm_mat4_mul(view, matrix, matrix);
			
			text_render_ext(strlit("Test Game Lobby!"), matrix, font, NULL, ALIGNMENT_CENTER);
		}
		
		// Draw Username
		{
			glm_mat4_identity(matrix);
			glm_translate(matrix, (vec3) { 0, -(f32)game.window.height * 0.25f });
			glm_scale(matrix, (vec3) { 2.0f, 2.0f });
			glm_mat4_mul(view, matrix, matrix);
			u8 text[512];
			usize len = 0;
			
			if (discord.connected) {
				len += snprintf(text, sizeof text, "Playing as %.*s#%u", strfmt(discord.user.username), discord.user.discriminator);
			} else {
				len += snprintf(text, sizeof text, "Connecting to Discord...");
			}
			
			string str = {
				.ptr = text,
				.len = len
			};
			
			text_render_ext(str, matrix, font, NULL, ALIGNMENT_CENTER);
		}
		
		b32 connected = discord_is_connected_to_lobby();
		scene_menu_draw_button(buttonCreate, mousePos,
							   connected ? strlit("Exit Lobby") : strlit("Create Lobby"),
							   view, font, false);
		b32 notConnected = !connected; // NOTE(luigi): TCC is going to crash if I don't do this
		scene_menu_draw_button(buttonPlay, mousePos, strlit("Start Game"), view, font, notConnected);
		scene_menu_draw_button(buttonExit, mousePos, strlit("Exit"), view, font, false);
		
		if (discord_is_connected_to_lobby()) {
			
			f32 y = 0.0f;
			
			for (usize i = 0; i < discord.lobbynet.userCount; ++i) {
				u8 text[512];
				usize len = 0;
				
				len += snprintf(text + len, sizeof text, "%c %i - %s#%s", discord.lobbynet.users[i].id == discord.user.id, i+1, discord.lobbynet.users[i].username, discord.lobbynet.users[i].discriminator);
				
				string str = { .ptr = text, .len = len };
				
				glm_mat4_identity(matrix);
				glm_translate(matrix, (vec3) { -(f32)game.window.width * 0.15f, y });
				glm_scale(matrix, (vec3) { 2.0f, 2.0f });
				glm_mat4_mul(view, matrix, matrix);
				text_render_ext(str, matrix, font, NULL, ALIGNMENT_NONE);
				
				y += 50.0f;
			}
			
		} else if (discord.joining) {
			glm_mat4_identity(matrix);
			//glm_translate(matrix, (vec3) {  });
			glm_scale(matrix, (vec3) { 2.0f, 2.0f });
			glm_mat4_mul(view, matrix, matrix);
			
			text_render_ext(strlit("Connecting to Lobby..."), matrix, font, NULL, ALIGNMENT_MIDDLE | ALIGNMENT_CENTER);
		}
		
		engine_end_frame();
	}
	
	game.currentScene = scene_game;
	
	return 0;
}