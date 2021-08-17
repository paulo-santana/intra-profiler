#include "mongoose.h"
#include <stdio.h>

static const char *g_listening_address = "http://localhost:8000";

static void callback(struct mg_connection *conn, int ev, void *ev_data, void *fn_data)
{
	if (ev != MG_EV_HTTP_MSG)
		return ;

	struct mg_http_message *msg = ev_data;
	if (mg_http_match_uri(msg, "/api/hello-world"))
		return mg_http_reply(conn, 200, "Content-Type: application/json\r\n", "{\"result\": %d}\n", 123);
	printf("data: %s\n", (char *)fn_data);
	return mg_http_reply(conn, 404, "", "%s", "Not found\n");
}

int main(void)
{
	struct mg_mgr mgr;
	mg_mgr_init(&mgr);
	mg_http_listen(&mgr, g_listening_address, callback, "hello world");
	for (;;)
		mg_mgr_poll(&mgr, 1000);
	mg_mgr_free(&mgr);
	return (0);
}
