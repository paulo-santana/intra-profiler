#ifndef PROFILER_H
# define PROFILER_H

#include "mongoose.h"
#define _GNU_SOURCE
#include <stdio.h>

typedef struct s_token {
	char *str;
	time_t expiration_date;
} t_token;

typedef struct s_api {
	int				keep_running;
	struct mg_mgr	mgr;
	t_token 		token;
} t_api;

typedef struct s_response {
	char *body;
	int body_len;
	int code;
} t_response;

typedef struct s_request {
	char *content;
	int content_len;
	char *url;
	t_api *api;
	t_response *response;
	int finished;
	char *method;
} t_request;

typedef struct s_student {
	char *login;
	char *first_name;
	char *url;
	char *image_url;
	char *pool_month;
	char *pool_year;
	int is_staff;
	int correction_points;
	int wallet;
	char *current_project;
	int avg_days_project;
} t_student;


void		connection_handler (struct mg_connection *conn, int event,
				void *event_data, void *data);

t_response	*request_intra(t_api *api, char *method, char *url, char *data);
void		free_request(t_request *req);
void		free_response(t_response *res);
void 		get_token(t_api *api);
void 		dump_response(t_response *response);
t_student	*get_student(const char *json);
char 		*student_to_json(t_student *student);
void		free_student(t_student *student);


#endif
