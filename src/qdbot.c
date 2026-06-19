#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "../include/qdbot.h"

#define API_PREFIX "[QDBotAPI] "

extern struct lws_protocols protocols[];
char cmdPrefix = '!';
typedef struct { char* cmd; void (*function)(); } qdbCmd;
qdbCmd commands[MAX_COMMANDS];
int logs = 0, commandCount = 0;
char TOKEN[128];
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) { (void)ptr; (void)userdata; return size * nmemb; }
// = = = = = PUBLIC = = = = = = = = = = = = = = =
void setLogs(int state) { logs = state; }
void madeBot(char* token) {
	strcpy(TOKEN, token);
	curl_global_init(CURL_GLOBAL_ALL);
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	struct lws_context *context = lws_create_context(&info);
	struct lws_client_connect_info i;
	memset(&i, 0, sizeof(i));
	i.context = context;
	i.address = "gateway.discord.gg";
	i.port = 443;
	info.pt_serv_buf_size = 65536;
	i.path = "/?v=10&encoding=json";
	i.host = "gateway.discord.gg";
	i.origin = "gateway.discord.gg";
	i.ssl_connection = LCCSCF_USE_SSL;
	i.protocol = protocols[0].name;
	i.pwsi = NULL;
	lws_client_connect_via_info(&i);
	while (1) { lws_service(context, 1000); }
	lws_context_destroy(context);
	curl_global_cleanup();
}
void sendMsg(const char *channel_id, const char *text) {
	CURL *curl = curl_easy_init();
	if(curl) {
		char url[256];
		sprintf(url, "https://discord.com/api/v10/channels/%s/messages", channel_id);
		char json_payload[512];
		sprintf(json_payload, "{\"content\":\"%s\"}", text);
		struct curl_slist *headers = NULL;
		char auth_header[256];
		snprintf(auth_header, sizeof(auth_header), "Authorization: Bot %s", TOKEN);
		headers = curl_slist_append(headers, auth_header);
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_perform(curl);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
}
void setCmdPrefix(char prefix) { cmdPrefix = prefix; }
void addCommand(char* command, void (*function)()) {
	if (commandCount >= MAX_COMMANDS) { printf(API_PREFIX"ERROR: Maximum number of commands exceeded!\n"); return; }
	strcpy(commands[commandCount].cmd, command);
	commands[commandCount].function = function;
	commandCount++;
}
// = = = = = END PUBLIC = = = = = = = = = = = = = = =
void extract_json_value(char *json, const char *key, char *output, size_t max_len) {
	char search_key[128];
	snprintf(search_key, sizeof(search_key), "\"%s\":\"", key);
	char* c = strstr(json, search_key);
	if (!c) return;
	c += strlen(search_key);
	char *end = strchr(c, '\"');
	if (!end) return;
	size_t len = end - c;
	if (len >= max_len) len = max_len - 1;
	strncpy(output, c, len);
	output[len] = '\0';
}
static int callback_discord(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	(void)user;
	switch (reason) {
		case LWS_CALLBACK_CLIENT_RECEIVE:
			if (logs >= 2) printf(API_PREFIX"%.*s\n", (int)len, (char *)in);
			if (strstr((char *)in, "\"op\":10")) {
				printf(API_PREFIX"Sending Identify...\n");
				char payload[1024];
				snprintf(payload, sizeof(payload), "{\"op\":2,\"d\":{\"token\":\"%s\",\"intents\":33281,\"properties\":{\"$os\":\"linux\",\"$browser\":\"my_bot\",\"$device\":\"my_bot\"}}}", TOKEN);
				size_t p_len = strlen(payload);
				unsigned char *buf = malloc(LWS_PRE + p_len);
				memcpy(buf + LWS_PRE, payload, p_len);
				lws_write(wsi, buf + LWS_PRE, p_len, LWS_WRITE_TEXT);
				free(buf);
			}
			if (strstr((char *)in, "\"t\":\"READY\"")) { printf(API_PREFIX"Logged!\n"); }
			if (strstr((char *)in, "\"op\":11")) {
				printf(API_PREFIX"ACK Recieved. Sending Heartbeat...\n");
				const char *hb = "{\"op\":1,\"d\":null}";
				size_t h_len = strlen(hb);
				unsigned char *bufh = malloc(LWS_PRE + h_len);
				memcpy(bufh + LWS_PRE, hb, h_len);
				lws_write(wsi, bufh + LWS_PRE, h_len, LWS_WRITE_TEXT);
				free(bufh);
			}
			if (strstr((char *)in, "\"t\":\"MESSAGE_CREATE\"")) {
				if (!strstr((char *)in, "\"bot\":true")) {
					char channel_id[32] = {0};
					char username[64] = {0};
					char cmd[64] = {0};
					extract_json_value((char *)in, "channel_id", channel_id, sizeof(channel_id));
					extract_json_value((char *)in, "username", username, sizeof(username));
					char *c = strstr((char *)in, "\"content\":\"");
					if (c) {
						c += 11;
						char *end = strchr(c, '\"');
						if (end) {
							size_t len_cmd = (size_t)(end - c);
							if (len_cmd > 63) len_cmd = 63;
							strncpy(cmd, c, len_cmd);
							if (logs >= 1) printf(API_PREFIX"New message:\n    Channel: %s\n    User: %s\n    Data: '%s'\n", channel_id, username, cmd);
							if (strcmp(cmd, "!help") == 0) {
								if (logs >= 1) printf(API_PREFIX"Responding...\n");
								sendMsg(channel_id, "Witaj! Jestem QBot.");
							} else if (strcmp(cmd, "elo") == 0) {
								if (logs >= 1) printf(API_PREFIX"Responding...\n");
								sendMsg(channel_id, "Siema!");
							}
						}
					}
				}
			}
			break;
			case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: printf(API_PREFIX"Connection error!\n"); break;
			default: break;
	}
	return 0;
}
struct lws_protocols protocols[] = { { .name = "discord-bot", .callback = callback_discord, .per_session_data_size = 0, .rx_buffer_size = 65536 }, { 0 } };
