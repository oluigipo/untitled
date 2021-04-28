#include "headers/all.h"
#include "discord_game_sdk.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

//- Constants and Macros
#define APPLICATION_ID 778719957956689922ll
#define assert_result(x) if ((x) != DiscordResult_Ok) { assert(! #x ); return false; }
#define assert_result_void(x) if ((x) != DiscordResult_Ok) { assert(! #x ); return; }

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
internal struct IDiscordCore* core = NULL;
internal struct IDiscordUserManager* users;
internal struct IDiscordAchievementManager* achievements;
internal struct IDiscordActivityManager* activities;
internal struct IDiscordRelationshipManager* relationships;
internal struct IDiscordApplicationManager* application;
internal struct IDiscordLobbyManager* lobbies;
internal DiscordBranch branch;

struct DiscordGlobalStuff discord = {
	.activity = {
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
	}
};

//- Callbacks
internal void discord_update_lobby_users(void) {
	if (discord.lobby.id == 0) return;
	
	i32 count;
	assert_result_void(lobbies->member_count(lobbies, discord.lobby.id, &count));
	if (count < 0) return;
	
	discord.lobbynet.userCount = (u32)count;
	for (i32 i = 0; i < count; ++i) {
		i64 id;
		
		assert_result_void(lobbies->get_member_user_id(lobbies, discord.lobby.id, i, &id));
		assert_result_void(lobbies->get_member_user(lobbies, discord.lobby.id, id, &discord.lobbynet.users[i]));
	}
}

internal void discord_callback_on_current_user_updated(void* data) {
	static struct DiscordUser user;
	DiscordResult result;
	
	result = users->get_current_user(users, &user);
	if (result != DiscordResult_Ok) {
		debug_error("Failed to get current user: %i\n", result);
		return;
	}
	
	discord.connected = true;
	discord.user.id = user.id;
	discord.user.username = (string) { .ptr = user.username, .len = strlen(user.username) };
	discord.user.discriminator = simple_parse_uint(user.discriminator, 8);
	
	debug_log("User ID: %llu\nUsername: %.*s\nDiscriminator: %u\n", discord.user.id, strfmt(discord.user.username), discord.user.discriminator);
}

internal void discord_callback_on_relationship_refresh(void* data) {
	debug_log("On Relationship Refresh\n");
}

internal void discord_callback_on_oauth2_token(void* data, DiscordResult result, struct DiscordOAuth2Token* token) {
	debug_log("Got OAuth2 Token response: %i\n", result);
}

internal void discord_callback_on_activity_update(void* data, DiscordResult result) {
	debug_log("Activity Update Status: %i\n", result);
}

internal void discord_callback_on_lobby_disconnect(void* data, DiscordResult result) {
	debug_log("Lobby Disconnect: %i\n", result);
	if (result != DiscordResult_Ok)
		return;
	
	discord.lobby.id = 0;
	discord.lobbynet.host = false;
	discord.lobbynet.userCount = 0;
	discord_update_activity();
}

internal void discord_callback_on_lobby_connect(void* data, DiscordResult result, struct DiscordLobby* lobby) {
	discord.joining = false;
	debug_log("Lobby Connect: %i\n", result);
	
	if (result != DiscordResult_Ok)
		return;
	
	if (lobbies->connect_network(lobbies, lobby->id) != DiscordResult_Ok ||
		lobbies->open_network_channel(lobbies, lobby->id, 0, false) != DiscordResult_Ok ||
		lobbies->open_network_channel(lobbies, lobby->id, 1, true) != DiscordResult_Ok
		) {
		lobbies->disconnect_lobby(lobbies, lobby->id, NULL, discord_callback_on_lobby_disconnect);
		return;
	}
	
	discord.lobby = *lobby;
	discord_update_lobby_users();
	discord_update_activity();
}

internal void discord_callback_on_lobby_create(void* data, DiscordResult result, struct DiscordLobby* lobby) {
	discord_callback_on_lobby_connect(data, result, lobby);
	
	discord.lobbynet.host = (lobby->owner_id == discord.user.id);
}

internal void discord_callback_on_lobby_update(void* data, i64 lobbyId) {
	debug_log("Lobby Update\n");;
	discord_update_lobby_users();
	discord_update_activity();
	
	if (lobbies->get_lobby(lobbies, lobbyId, &discord.lobby) != DiscordResult_Ok) {
		return;
	}
	
	discord.lobbynet.host = (discord.lobby.owner_id == discord.user.id);
}

internal void discord_callback_lobby_update(void* data, DiscordResult result) {
	debug_log("Lobby Update: %i\n", result);
	
	discord_callback_on_lobby_update(data, discord.lobby.id);
}

internal void discord_callback_on_lobby_delete(void* data, i64 lobbyId, u32 reason) {
	debug_log("Lobby Delete: %u\n", reason);
	
	discord.lobby.id = 0;
	discord.lobbynet.host = false;
	discord.lobbynet.userCount = 0;
	discord_update_activity();
}

internal void discord_callback_lobby_delete(void* data, DiscordResult result) {
	debug_log("Lobby Delete: %i\n", result);
	
	discord.lobby.id = 0;
	discord.lobbynet.host = false;
	discord.lobbynet.userCount = 0;
	discord_update_activity();
}

internal void discord_callback_on_network_message(void* _data, i64 lobbyId, i64 userId, u8 channel, u8* data, u32 length) {
	debug_log("Receiving buffer from: %lli\n", userId);
	
	Buffer buff = buffer_from(data, length);
	
	if (discord.on_buffer) {
		do {
			discord.on_buffer(userId, &buff);
		} while (buff.head != 0);
	}
}

internal void discord_callback_on_member_connect(void* data, i64 lobbyId, i64 userId) {
	debug_log("User Connected: %lli\n", userId);
	
	discord_update_lobby_users();
	discord_update_activity();
}

internal void discord_callback_on_member_update(void* data, i64 lobbyId, i64 userId) {
	debug_log("User Update: %lli\n", userId);
	
	discord_update_lobby_users();
	discord_update_activity();
}

internal void discord_callback_on_member_disconnect(void* data, i64 lobbyId, i64 userId) {
	debug_log("User Disconnect: %lli\n", userId);
	
	discord_update_lobby_users();
	discord_update_activity();
}

internal void discord_callback_lobby_disconnect_as_host(void* data, DiscordResult result) {
	if (result != DiscordResult_Ok) return;
	
	assert_result_void(lobbies->disconnect_network(lobbies, discord.lobby.id));
	lobbies->disconnect_lobby(lobbies, discord.lobby.id, data, discord_callback_on_lobby_disconnect);
}

internal void discord_callback_on_activity_join(void* data, const char* secret) {
	debug_log("On Activity Join: %s\n", secret);
	
	if (discord.lobby.id != 0 || discord.joining)
		return;
	
	discord.joining = true;
	lobbies->connect_lobby_with_activity_secret(lobbies, (char*)secret, NULL, discord_callback_on_lobby_connect);
}

internal DiscordResult discord_send_buffer_to_user(Buffer buff, b32 reliable, i64 userId) {
	debug_log("Sending buffer to: %lli\n", userId);
	
	return lobbies->send_network_message(lobbies, discord.lobby.id, userId, reliable, buff.data, buff.head);
}

//- Events
internal struct IDiscordUserEvents users_events = {
	.on_current_user_update = discord_callback_on_current_user_updated,
};
internal struct IDiscordRelationshipEvents relationships_events = {
	.on_refresh = discord_callback_on_relationship_refresh,
};
internal struct IDiscordLobbyEvents lobby_events = {
	.on_lobby_update = discord_callback_on_lobby_update,
	.on_lobby_delete = discord_callback_on_lobby_delete,
	.on_member_connect = discord_callback_on_member_connect,
	.on_member_update = discord_callback_on_member_update,
	.on_member_disconnect = discord_callback_on_member_disconnect,
	.on_network_message = discord_callback_on_network_message,
};
internal struct IDiscordActivityEvents activities_events = {
	.on_activity_join = discord_callback_on_activity_join
};

//- My Wrapper API

func b32 discord_init(void) {
	struct DiscordCreateParams params;
    DiscordResult result;
	
	// DiscordCreate
    DiscordCreateParamsSetDefault(&params);
    
	params.client_id = APPLICATION_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = &discord;
    params.activity_events = &activities_events;
    params.relationship_events = &relationships_events;
    params.user_events = &users_events;
	params.lobby_events = &lobby_events;
    
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

func void discord_deinit(void) {
	if (core) {
		core->destroy(core);
		core = NULL;
	}
}

func void discord_update(void) {
	if (core)
		core->run_callbacks(core);
}

func void discord_late_update(void) {
	if (core && lobbies)
		lobbies->flush_network(lobbies);
}

func void discord_update_activity(void) {
	if (discord.lobby.id != 0) {
		discord.activity.party.size.current_size = discord.lobbynet.userCount;
		discord.activity.party.size.max_size = discord.lobby.capacity;
		
		snprintf(discord.activity.party.id, sizeof discord.activity.party.id, "%lli", discord.lobby.id);
		
		char secret[128];
		assert_result_void(lobbies->get_lobby_activity_secret(lobbies, discord.lobby.id, &secret));
		
		strncpy(discord.activity.secrets.join, secret, 128);
	}
	
	activities->update_activity(activities, &discord.activity, NULL, discord_callback_on_activity_update);
}

func b32 discord_create_lobby(void) {
	if (discord.lobby.id != 0) return false;
	
	struct IDiscordLobbyTransaction* transaction;
	
	// get transaction
	assert_result(lobbies->get_lobby_create_transaction(lobbies, &transaction));
	
	// set things
	assert_result(transaction->set_capacity(transaction, DISCORD_MAX_USERS));
	assert_result(transaction->set_type(transaction, DiscordLobbyType_Private));
	assert_result(transaction->set_metadata(transaction, "version", "100"));
	
	lobbies->create_lobby(lobbies, transaction, NULL, discord_callback_on_lobby_create);
	discord.joining = true;
	discord.lobby.id = 0;
	
	return true;
}

func b32 discord_exit_lobby(void) {
	if (discord.lobby.id == 0) return false;
	
	if (!discord.lobbynet.host) {
		
		assert_result(lobbies->disconnect_network(lobbies, discord.lobby.id));
		lobbies->disconnect_lobby(lobbies, discord.lobby.id, NULL, discord_callback_on_lobby_disconnect);
		
	} else if (discord.lobbynet.userCount > 1) {
		struct IDiscordLobbyTransaction* transaction;
		
		assert_result(lobbies->get_lobby_update_transaction(lobbies, discord.lobby.id, &transaction));
		assert_result(transaction->set_owner(transaction, discord.lobbynet.users[1].id));
		
		lobbies->update_lobby(lobbies, discord.lobby.id, transaction, NULL, discord_callback_lobby_disconnect_as_host);
		
	} else {
		assert_result(lobbies->disconnect_network(lobbies, discord.lobby.id));
		lobbies->delete_lobby(lobbies, discord.lobby.id, NULL, discord_callback_lobby_delete);
	}
	
	return true;
}

func b32 discord_toggle_lobby_lock(void) {
	if (discord.lobby.id == 0) return false;
	
	struct IDiscordLobbyTransaction* transaction;
	
	assert_result(lobbies->get_lobby_update_transaction(lobbies, discord.lobby.id, &transaction));
	assert_result(transaction->set_locked(transaction, !discord.lobby.locked));
	
	lobbies->update_lobby(lobbies, discord.lobby.id, transaction, NULL, discord_callback_lobby_update);
	
	return true;
}

func b32 discord_send_buffer(Buffer buff, b32 reliable) {
	if (discord.lobby.id == 0 || discord.joining) return false;
	
	reliable = !!reliable; // just to be safe
	
	if (!discord.lobbynet.host)
		return discord_send_buffer_to_user(buff, reliable, discord.lobby.owner_id);
	
	for (usize i = 0; i < discord.lobbynet.userCount; ++i) {
		if (discord.lobbynet.users[i].id == discord.user.id) continue;
		
		assert_result(discord_send_buffer_to_user(buff, reliable, discord.lobbynet.users[i].id));
	}
	
	return true;
}

func b32 discord_is_connected_to_lobby(void) {
	return discord.connected && discord.lobby.id != 0;
}

func b32 discord_am_the_owner(void) {
	return discord_is_connected_to_lobby() && discord.lobbynet.host;
}

#undef assert_result
