#include "profiler.h"

static const char *g_listening_address = "http://localhost:8000";

static void handle_request(struct mg_connection *conn, struct mg_http_message *msg)
{
	if (strncmp(msg->method.ptr, "GET", 3) != 0)
		return mg_http_reply(conn, 405, "", "");
	return mg_http_reply(conn, 200, "", "hello again, world\n");
}

static void callback(struct mg_connection *conn, int ev, void *ev_data, void *fn_data)
{
	if (ev != MG_EV_HTTP_MSG)
		return ;

	(void)fn_data;
	struct mg_http_message *msg = ev_data;
	if (mg_http_match_uri(msg, "/api"))
		return mg_http_reply(conn, 200, "", "bom dia!");
	if (mg_http_match_uri(msg, "/api/*"))
		return (handle_request(conn, msg));
	else
		return mg_http_reply(conn, 404, "", "%s", "Not found\n");
}

void init_api(t_api *api)
{
	api->keep_running = 1;
	mg_mgr_init(&api->mgr);
	mg_http_listen(&api->mgr, g_listening_address, callback, "hello world");
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
