#include "mjson.h"
#include "profiler.h"

static const char *g_listening_address = "http://0.0.0.0:80";
#define API42 "https://api.intra.42.fr"

static char *manage_student_data(char *raw_data)
{
	char *student_json;

	t_student *student = get_student(raw_data);
	student_json = student_to_json(student);
	free_student(student);
	printf("%s\n", student_json);
	return (student_json);
}

void fetch_user(t_api *api, char *url, void* current_connection)
{
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
	if (res->code == 200)
	{
		char *result = manage_student_data(res->body);
		mg_http_reply(current_connection, 200,
				"Content-Type: application/json\r\n", "%s\r\n", result);
		free(result);
	}
	else
		mg_http_reply(current_connection, res->code,
				"", "%s\r\n", res->body);
	free_response(res);
}

static void handle_request(t_api *api, struct mg_connection *conn, struct mg_http_message *msg)
{
	char username[9];
	char url[42];

	int prefix_len = strlen("/api/v1/");
	if (msg->uri.len <= 8 || msg->uri.len > 16)
		return (mg_http_reply(conn, 404, "", ""));
	if (strncmp(msg->method.ptr, "GET", 3) == 0)
	{
		bzero(username, 9);
		bzero(url, 42);
		snprintf(username, 9, "%.*s", 
				(int) msg->uri.len - prefix_len, msg->uri.ptr + prefix_len);
		snprintf(url, 42, "%s/v2/users/%s", API42, username);
		fetch_user(api, url, conn);
	}
	else
		return mg_http_reply(conn, 405, "", "");
}

static void callback(struct mg_connection *conn, int ev, void *ev_data, void *api)
{
	if (ev == MG_EV_ERROR)
		return mg_http_reply(conn, 500, "", "");
	if (ev != MG_EV_HTTP_MSG)
		return ;

	struct mg_http_message *msg = ev_data;

	if (mg_http_match_uri(msg, "/api/v1"))
		return mg_http_reply(conn, 200, "", "bom dia!");

	if (mg_http_match_uri(msg, "/api/v1/*"))
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
