#include "profiler.h"

static const char *g_listening_address = "http://localhost:8000";
static const char *token = "MY_TOKEN";
#define API42 "https://api.intra.42.fr"
#define USERS "/v2/users/psergio-"

void api_handler(struct mg_connection *conn, int ev,
                                   void *ev_data, void *curr_connection)
{
	if (ev == MG_EV_CONNECT)
	{
		// Connected to server. Extract host name from URL
		struct mg_str host = mg_url_host(API42 USERS);

		// If s_url is https://, tell client connection to use TLS
		if (mg_url_is_ssl(API42 USERS)) {
			struct mg_tls_opts opts = {.ca = "ca.pem", .srvname = host};
			mg_tls_init(conn, &opts);
		}

		mg_printf(conn,
				"GET %s HTTP/1.0\n"
				"Host: %.*s\n"
				"Authorization: Bearer %s\n"
				"Accept: */*\n"
				"\n",
				mg_url_uri(API42 USERS), (int) host.len, host.ptr, token);
	}
	else if (ev == MG_EV_HTTP_MSG)
	{
		struct mg_http_message *hm = (struct mg_http_message *) ev_data;
		mg_http_reply(curr_connection, 200, "Content-Type: application/json\n", "%.*s", hm->body.len, hm->body.ptr);
	}
	else if (ev == MG_EV_ERROR)
	{
		printf("error\n");
	}
}

void fetch(t_api *api, char *url, void* current_connection) {
	mg_http_connect(&api->mgr, url, api_handler, current_connection);
}

static void handle_request(t_api *api, struct mg_connection *conn, struct mg_http_message *msg)
{
	if (strncmp(msg->method.ptr, "GET", 3) == 0)
		fetch(api, API42, conn);
	else
		return mg_http_reply(conn, 405, "", "method: %.*s\n", msg->method.len, msg->method.ptr);
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
		return mg_http_reply(conn, 404, "", "%s", "Not found\n");
}

void init_api(t_api *api)
{
	api->keep_running = 1;
	mg_mgr_init(&api->mgr);
	mg_http_listen(&api->mgr, g_listening_address, callback, api);
}

void start_server(t_api *api)
{
	while(api->keep_running)
		mg_mgr_poll(&api->mgr, 1000);
}

int main(void)
{
	t_api api;

	init_api(&api);
	start_server(&api);
	mg_mgr_free(&api.mgr);
	return (0);
}
