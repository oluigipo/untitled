#include "headers/all.h"
#include "discord_game_sdk.h"
#include <time.h>
#include <string.h>

//- Constants
#define APPLICATION_ID 778719957956689922ll

//- Helper Functions
internal u32 simple_parse_uint(const char* restrict str, uint max) {
	u32 r = 0;
	
	while (*str && max--) {
		r *= 10;
		r += *str - '0';
		++str;
	}
	
	return r;
}

internal u64 get_unix_timestamp(void) {
	time_t result = time(NULL);
	if (result == -1)
		return 0;
	
	return (u64)result;
}

//- Types
enum EDiscordResult typedef DiscordResult;

//- Globals
internal struct IDiscordCore* core;
internal struct IDiscordUserManager* users;
internal struct IDiscordAchievementManager* achievements;
internal struct IDiscordActivityManager* activities;
internal struct IDiscordRelationshipManager* relationships;
internal struct IDiscordApplicationManager* application;
internal struct IDiscordLobbyManager* lobbies;

internal struct IDiscordUserEvents users_events;
internal struct IDiscordActivityEvents activities_events;
internal struct IDiscordRelationshipEvents relationships_events;
internal DiscordBranch branch;

struct DiscordClientUser discord;

//- Callbacks
internal void discord_callback_on_user_updated(void* data) {
	static struct DiscordUser user;
	DiscordResult result;
	
	result = users->get_current_user(users, &user);
	if (result != DiscordResult_Ok) {
		debug_error("Failed to get current user: %i\n", result);
		return;
	}
	
	discord.connected = true;
	discord.id = user.id;
	discord.username = (string) { .ptr = user.username, .len = strnlen(user.username, sizeof user.username) };
	discord.discriminator = simple_parse_uint(user.discriminator, 8);
	
	debug_log("User ID: %llu\nUsername: %.*s\nDiscriminator: %u\n", discord.id, discord.username.len, discord.username.ptr, discord.discriminator);
}

internal void discord_callback_on_relationship_refresh(void* data) {
	// ...
}

internal void discord_callback_on_oauth2_token(void* data, DiscordResult result, struct DiscordOAuth2Token* token) {
	debug_log("Got OAuth2 Token response: %i\n", result);
}

internal void discord_callback_on_activity_update(void* data, DiscordResult result) {
	debug_log("Activity Update Status: %i\n", result);
}

//- My Wrapper API
b32 discord_init(void) {
    struct DiscordCreateParams params;
    DiscordResult result;
	
	// some callbacks
	users_events.on_current_user_update = discord_callback_on_user_updated;
	relationships_events.on_refresh = discord_callback_on_relationship_refresh;
	
	// DiscordCreate
    DiscordCreateParamsSetDefault(&params);
    
	params.client_id = APPLICATION_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = &discord;
    params.activity_events = &activities_events;
    params.relationship_events = &relationships_events;
    params.user_events = &users_events;
    
	result = DiscordCreate(DISCORD_VERSION, &params, &core);
	if (result != DiscordResult_Ok || !core)
		return false;
	
	// get managers
	users         = core->get_user_manager(core);
	achievements  = core->get_achievement_manager(core);
	activities    = core->get_activity_manager(core);
	application   = core->get_application_manager(core);
	lobbies       = core->get_lobby_manager(core);
	relationships = core->get_relationship_manager(core);
	
#if 0
	// NOTE(luigi): disabled. asking for oauth2 token can be an inconvenience for the end user.
	//              I don't know if we are going to need this, but it'll be disabled for now.
	application->get_oauth2_token(application, NULL, discord_callback_on_oauth2_token);
	
	// get branch (idk what this is)
	application->get_current_branch(application, &branch);
	debug_log("Current branch %s\n", branch);
#endif
	
	discord_update_activity();
	
	return true;
}

void discord_deinit(void) {
	if (core) {
		core->destroy(core);
		core = NULL;
	}
}

void discord_update(void) {
	if (core)
		core->run_callbacks(core);
}

void discord_update_activity(void) {
	struct DiscordActivity activity = {
		.type = DiscordActivityType_Playing,
		.application_id = APPLICATION_ID,
		.name = "Test 1",
		.state = "Test 2",
		.details = "Test 3",
		.assets = {
			.large_image = "eredin",
			.large_text = "large text",
			.small_image = "preto",
			.small_text = "small text"
		}
	};
	
	activities->update_activity(activities, &activity, NULL, discord_callback_on_activity_update);
}

