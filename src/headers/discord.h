#pragma once
#include "types.h"

struct DiscordClientUser {
	string username;
	i64 id;
	u32 discriminator;
	b32 connected;
};

global struct DiscordClientUser discord;

b32 discord_init(void);
void discord_update(void);
void discord_deinit(void);
void discord_update_activity(void);
