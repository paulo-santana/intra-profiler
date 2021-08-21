#include "mongoose.h"
#include "profiler.h"

void send_request(struct mg_connection *conn, t_request *request)
{
	struct mg_str host = mg_url_host(request->url);
	if (mg_url_is_ssl(request->url))
	{
		struct mg_tls_opts opts = { .srvname = host };
		mg_tls_init(conn, &opts);
	}
	mg_printf(conn,
			"%s %s HTTP/1.0\r\n"
			"Host: %.*s\r\n"
			"Authorization: Bearer %s\r\n"
			"Content-Length: %d\r\n"
			"Accept: */*\r\n"
			"\r\n"
			"%s\r\n",
			request->method, mg_url_uri(request->url),
			(int) host.len, host.ptr,
			request->api->token.str,
			request->content_len,
			request->content);
}

void get_response(struct mg_http_message *msg, t_response *response)
{
	response->body = strndup(msg->body.ptr, msg->body.len);
	response->body_len = msg->body.len;
	response->code = atoi(msg->uri.ptr);
}

void connection_handler (struct mg_connection *conn, int event,
		void *event_data, void *fn_data)
{
	t_request *request = fn_data;

	if (event == MG_EV_CONNECT)
		send_request(conn, request);
	else if (event == MG_EV_HTTP_MSG)
	{
		get_response(event_data, request->response);
		request->finished = 1;
	}
	else
	{
		if (event == MG_EV_ERROR)
		{
			printf("error\n");
			request->finished = 1;
		}
		else if (event == MG_EV_CLOSE)
			request->finished = 1;
		printf("event: %d\n", event);
	}
}

t_response *request_intra(t_api *api, char *method, char *url, char *data)
{
	t_response *response;
	t_request *request;

	response = calloc(1, sizeof(t_response));
	request = calloc(1, sizeof(t_request));

	request->url = url;
	request->content= data;
	request->content_len = strlen(data);
	request->response = response;
	request->method = method;
	request->api = api;

	mg_http_connect(&api->mgr, url, connection_handler, request);
	while (!request->finished)
		mg_mgr_poll(&api->mgr, 100);
	free_request(request);
	return response;
}

void free_request(t_request *req)
{
	free(req);
}

void free_response(t_response *res)
{
	free(res->body);
	free(res);
}
