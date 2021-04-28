#pragma once
#include "types.h"
#include "buffer.h"
#include <discord_game_sdk.h>

#define DISCORD_MAX_USERS 4

//- Basics
struct DiscordClientUser {
	string username;
	i64 id;
	u32 discriminator;
};

typedef void (*DiscordBufferCallback)(i64 user, Buffer* buffer);

// Globals
global struct DiscordGlobalStuff {
	b32 connected;
	b32 joining;
	DiscordBufferCallback on_buffer;
	
	struct DiscordActivity activity;
	struct DiscordLobby lobby;
	struct DiscordClientUser user;
	struct {
		b32 host;
		
		struct DiscordUser users[DISCORD_MAX_USERS];
		u32 userCount;
	} lobbynet;
} discord;

//- API
func b32 discord_init(void);
func void discord_update(void);
func void discord_late_update(void);
func void discord_deinit(void);
func void discord_update_activity(void);

// Lobby things
func b32 discord_create_lobby(void);
func b32 discord_exit_lobby(void);
func b32 discord_send_buffer(Buffer buff, b32 reliable);
func b32 discord_toggle_lobby_lock(void);
func b32 discord_is_connected_to_lobby(void);
func b32 discord_am_the_owner(void);
