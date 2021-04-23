#pragma once
#include "types.h"
#include "buffer.h"
#include <discord_game_sdk.h>

#define DISCORD_MAX_USERS 6

//- Basics
struct DiscordClientUser {
	string username;
	i64 id;
	u32 discriminator;
};

// Globals
global struct DiscordGlobalStuff {
	b32 connected;
	b32 joining;
	void (*on_buffer)(void*, i64, i64, u8, Buffer*);
	
	struct DiscordLobby lobby;
	struct DiscordClientUser user;
	struct {
		b32 host;
		
		struct DiscordUser users[DISCORD_MAX_USERS];
		u32 userCount;
	} lobbynet;
} discord;

//- API
b32 discord_init(void);
void discord_update(void);
void discord_late_update(void);
void discord_deinit(void);
void discord_update_activity(void);

// Lobby things
b32 discord_create_lobby(void);
b32 discord_exit_lobby(void);
b32 discord_send_buffer(Buffer buff, b32 reliable);
b32 discord_toggle_lobby_lock(void);
