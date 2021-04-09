#include "headers/all.h"
#include "discord_game_sdk.h"

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

//- Types
struct DiscordApplication {
	struct IDiscordCore* core;
	struct IDiscordUserManager* users;
	struct IDiscordAchievementManager* achievements;
	struct IDiscordActivityManager* activities;
	struct IDiscordRelationshipManager* relationships;
	struct IDiscordApplicationManager* application;
	struct IDiscordLobbyManager* lobbies;
	DiscordUserId user_id;
};

enum EDiscordResult typedef DiscordResult;

//- Globals
internal struct DiscordApplication discord;
internal struct IDiscordUserEvents users_events;
internal struct IDiscordActivityEvents activities_events;
internal struct IDiscordRelationshipEvents relationships_events;
internal DiscordBranch branch;
struct DiscordClientUser discord_user;

//- Callbacks
internal void discord_callback_on_user_updated(void* data) {
	static struct DiscordUser user;
	DiscordResult result;
	
	result = discord.users->get_current_user(discord.users, &user);
	if (result != DiscordResult_Ok) {
		debug_error("Failed to get current user: %i\n", result);
		return;
	}
	
	discord_user.connected = true;
	discord_user.id = user.id;
	discord_user.username = (string) { .ptr = user.username, .len = strlen(user.username) };
	discord_user.discriminator = simple_parse_uint(user.discriminator, 8);
	
	debug_log("User ID: %llu\nUsername: %.*s\nDiscriminator: %u\n", discord_user.id, discord_user.username.len, discord_user.username.ptr, discord_user.discriminator);
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
    
	result = DiscordCreate(DISCORD_VERSION, &params, &discord.core);
	if (result != DiscordResult_Ok || !discord.core)
		return false;
	
	// get managers
	discord.users = discord.core->get_user_manager(discord.core);
	discord.achievements = discord.core->get_achievement_manager(discord.core);
	discord.activities = discord.core->get_activity_manager(discord.core);
	discord.application = discord.core->get_application_manager(discord.core);
	discord.lobbies = discord.core->get_lobby_manager(discord.core);
	discord.relationships = discord.core->get_relationship_manager(discord.core);
	
#if 0
	// NOTE(luigi): disabled. asking for oauth2 token can be an inconvenience for the end user.
	//              I don't know if we are going to need this, but it'll be disabled for now.
	discord.application->get_oauth2_token(discord.application, NULL, discord_callback_on_oauth2_token);
#endif
	
	// get branch (idk what this is)
	discord.application->get_current_branch(discord.application, &branch);
	debug_log("Current branch %s\n", branch);
	
	discord_update_activity();
	
	return true;
}

void discord_deinit(void) {
	if (discord.core) {
		discord.core->destroy(discord.core);
		discord.core = NULL;
	}
}

void discord_update(void) {
	if (discord.core)
		discord.core->run_callbacks(discord.core);
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
	
	discord.activities->update_activity(discord.activities, &activity, NULL, discord_callback_on_activity_update);
}

