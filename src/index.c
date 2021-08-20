#include "profiler.h"

static const char *g_listening_address = "http://localhost:8000";
#define API42 "https://api.intra.42.fr"

void fetch(t_api *api, char *url, void* current_connection) {
	(void)url;

	get_token(api);

	if (api->token.str == NULL)
	{
		mg_http_reply(current_connection, 500,
				"Content-Type: application/json\r\n",
				"{\"error\":\"Failed to get a token\"}"
				"\r\n");
		printf("failed to get a token\n");
		return ;
	}
	t_response *res = request_intra(api, "GET", url, "");
	mg_http_reply(current_connection, res->code,
			"Content-Type: application/json\r\n", "%s\r\n", res->body);
	free_response(res);
}

static void handle_request(t_api *api, struct mg_connection *conn, struct mg_http_message *msg)
{
	char username[9];
	char url[42];

	if (msg->uri.len <= 5)
		return (mg_http_reply(conn, 400, "", ""));
	if (strncmp(msg->method.ptr, "GET", 3) == 0)
	{
		bzero(username, 9);
		bzero(url, 42);
		snprintf(username, 9, "%.*s", (int) msg->uri.len - 5, msg->uri.ptr + 5);
		snprintf(url, 42, "%s/v2/users/%s", API42, username);
		fetch(api, url, conn);
	}
	else
		return mg_http_reply(conn, 405, "", "");
}

static void callback(struct mg_connection *conn, int ev, void *ev_data, void *api)
{
	if (ev != MG_EV_HTTP_MSG)
		return ;

	struct mg_http_message *msg = ev_data;

	if (mg_http_match_uri(msg, "/api"))
		return mg_http_reply(conn, 200, "", "bom dia!");

	if (mg_http_match_uri(msg, "/api/*"))
		return (handle_request(api, conn, msg));

	else
		return mg_http_reply(conn, 404, "", "");
}

void init_api(t_api *api)
{
	api->keep_running = 1;
	api->token.str = NULL;
	api->token.expiration_date = 0;
	mg_mgr_init(&api->mgr);
	mg_http_listen(&api->mgr, g_listening_address, callback, api);
}

void start_server(t_api *api)
{
	while(api->keep_running)
		mg_mgr_poll(&api->mgr, 100);
}

int main(void)
{
	t_api api;

	init_api(&api);
	start_server(&api);
	mg_mgr_free(&api.mgr);
	return (0);
}
