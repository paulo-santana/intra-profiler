#ifndef PROFILER_H
# define PROFILER_H

#include "mongoose.h"
#define _GNU_SOURCE
#include <stdio.h>

typedef struct s_api {
	int				keep_running;
	struct mg_mgr	mgr;
	char *token;
} t_api;

typedef struct s_response {
	char *body;
	int body_len;
	int code;
} t_response;

typedef struct s_request {
	char *body;
	int body_len;
	char *url;
	t_api *api;
	t_response *response;
	int finished;
	char *method;
} t_request;


void		connection_handler (struct mg_connection *conn, int event,
				void *event_data, void *data);

t_response	*request_intra(t_api *api, char *method, char *url, char *data);
void		free_request(t_request *req);
void		free_response(t_response *res);
void 		get_token(t_api *api);
void 		dump_response(t_response *response);

#endif
