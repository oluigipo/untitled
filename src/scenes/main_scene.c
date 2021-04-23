#include "headers/all.h"

/*

 Game Ideas:
- Turn-based
- Maybe multiplayer
- "War Groove"-kinda style
- "pique bandeirinha"

*/

uint scene_main(void) {
	mat4 object;
	
	// Walle things
	vec2 position = { 0 };
	vec2 velocity = { 0 };
	Sprite walleSprite = {
		.texture = &assets_textures[TEX_SPRITES_0],
		.offset = { 0, 16 },
		.size = { 128, 128 }
	};
	
	// Particles
	ParticleManager mgr = { 0 };
	
	// Camera
	struct Camera camera = {
		.pos = { position[0], position[1] },
		.angle = 0,
		.zoom = 2.0f / SCREEN_SCALE,
		.speed = 0.3f
	};
	
	// Music
	uint buffer = sound_load_file("assets/test.ogg");
	uint source = sound_make_source();
	
	sound_source_buffer(source, buffer);
	sound_source_attenuation(source, 1.0f, 300.0f, 10000.0f);
	sound_source_params(source, 0.2f, 1.0f);
	sound_source_position(source, (vec3) { 0, 0, -300.0f });
	sound_play_source(source);
	
	b32 playingSound = true;
	
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
	
	// Ghosts
	struct { vec3 position; u32 blend; } ghosts[100];
	const usize ghostCount = sizeof(ghosts) / sizeof(ghosts[0]);
	
	Sprite ghostSprite = {
		.texture = &assets_textures[TEX_SPRITES_0],
		.offset = { 0, 0 },
		.size = { 16, 16 }
	};
	
	for (uint i = 0; i < ghostCount; ++i) {
		ghosts[i].position[0] = (random_f64() * 2 - 1) * 500.0f;
		ghosts[i].position[1] = (random_f64() * 2 - 1) * 500.0f;
		ghosts[i].position[2] = 0;
		
		switch (random_u32() % 3) {
			case 0: ghosts[i].blend = 0xFF8888FF; break;
			case 1: ghosts[i].blend = 0xFF88FF88; break;
			case 2: ghosts[i].blend = 0xFFFF8888; break;
		}
	}
	
	// Game Loop
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		//- Update
		if (gamepad_is_pressed(GPAD_BUTTON_Y)) {
			playingSound = !playingSound;
			
			if (playingSound) sound_play_source(source);
			else sound_pause_source(source);
		}
		
		if (gamepad_is_pressed(GPAD_BUTTON_START))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		f32 targetSpeed = 3.0f + 2.0f * gamepad_axis(GPAD_AXIS_R2);
		velocity[0] = lerpf(velocity[0], gamepad_axis(GPAD_AXIS_LX) * targetSpeed, 0.3f * game.deltaTime);
		velocity[1] = lerpf(velocity[1], gamepad_axis(GPAD_AXIS_LY) * targetSpeed, 0.3f * game.deltaTime);
		
		position[0] += velocity[0] * game.deltaTime;
		position[1] += velocity[1] * game.deltaTime;
		
		sound_listener_position((vec3) { position[0], position[1] });
		
		if (gamepad_is_down(GPAD_BUTTON_A)) {
			f32 scale = random_f64() * 0.5f + 0.5f;
			
			partmgr_add(&mgr, PART_SIMPLE, &(struct PartSimple) {
							.pos = { position[0], position[1] },
							.scale = { scale, scale },
							.speed = { random_f64() * 18.0 - 8.0f, random_f64() * 16.0f - 8.0f },
							.color = { random_f64(), random_f64(), random_f64() },
							.alpha = 1.0f,
							.angle = random_f64() * TAU
						});
		}
		
		partmgr_update(&mgr);
		
		glm_vec2_copy(position, camera.targetPos);
		camera_update(&camera);
		
		//- Draw
		glClearColor(0.0f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		mat4 view;
		camera_matrix(&camera, view);
		
		// Draw Tilemap
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { -200.0f, -200.0f });
		glm_mat4_mul(view, object, object);
		tilemap_render(&tilemap, object);
		
		// Sprite
		for (uint i = 0; i < ghostCount; ++i) {
			glm_mat4_identity(object);
			glm_translate(object, ghosts[i].position);
			glm_scale(object, (vec3) { 2, 2 });
			glm_mul(view, object, object);
			
			sprite_batch_add(NULL, &ghostSprite, object, ghosts[i].blend, 0, ALIGNMENT_NONE);
		}
		
		// Draw walle
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { position[0], position[1] });
		glm_mat4_mul(view, object, object);
		
		sprite_batch_add(NULL, &walleSprite, object, 0xFFFFFFFF, 0, ALIGNMENT_CENTER | ALIGNMENT_MIDDLE);
		
		// Finish Sprite Batching
		sprite_batch_flush(NULL);
		
		// Draw text
		unsigned char myText[512];
		string str = { .ptr = myText };
		str.len = snprintf(myText, sizeof myText,
						   "%.*s \x01\n",
						   strfmt(locale_str(TXT_HELLO_WORLD)));
		
		str.len += snprintf(myText + str.len, sizeof(myText) - str.len,
							(discord.connected) ? "Discord: %.*s#%u" : "Connecting to Discord...",
							discord.user.username.len, discord.user.username.ptr, discord.user.discriminator);
		
		vec2u textusize;
		text_size(str, textusize, &assets_textures[TEX_DEFAULT_FONT]);
		vec2 textsize = { textusize[0], textusize[1] };
		glm_vec2_scale(textsize, 2.25f, textsize);
		
		glm_mat4_identity(object);
		glm_scale(object, textsize);
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		glm_mat4_mul(view, object, object);
		primitive_render_roundrect(object, 0xFF202020, 1.0f, (vec2) { 1, textsize[1] / textsize[0] });
		
		glm_mat4_identity(object);
		glm_scale(object, (vec3) { 2, 2 });
		glm_mul(view, object, object);
		text_render_ext(str, object, &assets_textures[TEX_DEFAULT_FONT], NULL, ALIGNMENT_CENTER | ALIGNMENT_MIDDLE);
		
		partmgr_render(&mgr, view);
		
		engine_end_frame();
	}
	
	sound_delete_source(source);
	sound_unload(buffer);
	
	// Close the game
	game.currentScene = NULL;
	
	return 0;
}
