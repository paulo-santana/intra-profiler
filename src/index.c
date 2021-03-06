#include "mjson.h"
#include "profiler.h"

#define LISTENING_ADDRESS "http://0.0.0.0:"
#define API42 "https://api.intra.42.fr"

static char *manage_student_data(t_api *api, char *raw_data)
{
	char *student_json;

	t_student *student = get_student(raw_data);
	student_json = student_to_json(student);
	save_student(api->mongo, student->intra_id, student);
	free_student(student);
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
		char *result = manage_student_data(api, res->body);
		mg_http_reply(current_connection, 200,
				"Content-Type: application/json\r\n", "%s\r\n", result);
		free(result);
	}
	else
		mg_http_reply(current_connection, res->code, "", "%s\r\n", res->body);
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

char *get_listening_addr(const char *port)
{
	char *addr;

	addr = malloc(100);
	if (addr == NULL)
		return (NULL);
	strncpy(addr, LISTENING_ADDRESS, 94);
	strncat(addr, port, strlen(addr) + 6);
	return (addr);
}

void init_api(t_api *api)
{
	api->token.str = NULL;
	api->token.expiration_date = 0;

	char *port = getenv("PORT");
	api->keep_running = 1;
	if (port == NULL)
		port = "8000";
	api->port = port;
	api->listening_address = get_listening_addr(port);
	mg_mgr_init(&api->mgr);
	mg_http_listen(&api->mgr, api->listening_address, callback, api);
}

void start_server(t_api *api)
{
	while(api->keep_running)
		mg_mgr_poll(&api->mgr, 100);
}

void close_api(t_api *api)
{
	free(api->token.str);
	free(api->listening_address);
}

int main(void)
{
	t_api api;
	t_mongo mongo;

	if(!init_db(&mongo))
	{
		fprintf(stderr, "failed to connect to the database\n");
		return (0);
	}
	init_api(&api);
	api.mongo = &mongo;
	start_server(&api);
	mg_mgr_free(&api.mgr);
	close_db(&mongo);
	close_api(&api);
	return (0);
}
