#include "mjson.h"
#include "math.h"
#include "profiler.h"

static char *get_property_str(const char *property, const char *json, int len)
{
	const char *str;

	int name_len;
	if(mjson_find(json, len, property, &str, &name_len))
		return(strndup(str, name_len));
	return (NULL);
}

static int get_property_int(const char *property, const char *json, int len)
{
	double value;

	value = -1;
	mjson_get_number(json, len, property, &value);
	return ((int) value);
}

static int get_property_bool(const char *property, const char *json, int len)
{
	int value;

	value = 1;
	mjson_get_bool(json, len, property, &value);
	return (value);
}

static char *get_current_project(const char *json, int len)
{
	const char *project;
	const char *status;
	int status_len;
	int project_len;

	project_len = 0;
	if(mjson_find(json, len, "$.projects_users[0]", &project, &project_len))
	{
		if(mjson_find(project, project_len, "$.status", &status, &status_len))
		{
			if (strncmp(status, "finished", 8) == 0)
				return strdup("Course finished");
			else
			{
				if(mjson_find(project, project_len, "$.project.name", &project, &project_len))
					return (strndup(project, project_len));
			}
		}
	}
	return (NULL);
}

// str == "2021-08-07T08:53:59.776Z"
// year-month-day, hour, minute, second
time_t str_to_time(const char *str)
{
	time_t result = 0;
	int year, month, day, hour, minute;
	(void)str;
	if (sscanf(str, "%4d-%2d-%2dT%2d:%2d",
				&year, &month, &day, &hour, &minute) == 5)
	{
		struct tm breakdown = {0};
		breakdown.tm_year = year - 1900;
		breakdown.tm_mon = month;
		breakdown.tm_mday = day;
		breakdown.tm_hour = hour;
		breakdown.tm_min = minute;
		result = mktime(&breakdown);
		if (result == -1)
		{
			printf("Failed to convert time input to time_t\n");
		}
	}
	else
	{
		printf("The input was not a valid time format\n");
	}
	return result;
}

static int get_avg_days_project(const char *json, int len)
{
	const char *project;
	int iterator = 0;
	int project_len;
	char search_path[100];
	char str_start[100];
	char str_end[100];
	int i = 0;
	int total = 0;
	int finished = 0;
	double cursus_id;

	snprintf(search_path, 100, "$.projects_users[%d]", iterator++);
	while(mjson_find(json, len, search_path, &project, &project_len))
	{
		snprintf(search_path, 100, "$.projects_users[%d]", iterator++);
		mjson_get_bool(project, project_len, "$.marked", &finished);
		mjson_get_number(project, project_len, "$.cursus_ids[0]", &cursus_id);
		if (!finished || cursus_id != 21)
			continue;
		mjson_get_string(project, project_len, "$.created_at", str_start, 100);
		mjson_get_string(project, project_len, "$.marked_at", str_end, 100);
		time_t start = str_to_time(str_start);
		time_t end = str_to_time(str_end);
		total += end - start;
		i++;
	}
	return (round((double)total / 60 / 60 / 24 / i));
}

t_student *get_student(const char *json)
{
	int json_len;
	t_student *student;

	json_len = strlen(json);
	student = calloc(1, sizeof(t_student));
	if (student == NULL)
		return (NULL);
	student->intra_id = get_property_int("$.id", json, json_len);
	student->login = get_property_str("$.login", json, json_len);
	student->first_name = get_property_str("$.first_name", json, json_len);
	student->url = get_property_str("$.url", json, json_len);
	student->image_url = get_property_str("$.image_url", json, json_len);
	student->pool_month = get_property_str("$.pool_month", json, json_len);
	student->pool_year = get_property_str("$.pool_year", json, json_len);
	student->correction_points = get_property_int("$.correction_point", json, json_len);
	student->wallet = get_property_int("$.wallet", json, json_len);
	student->is_staff = get_property_bool("$.staff?", json, json_len);
	student->current_project = get_current_project(json, json_len);
	student->avg_days_project = get_avg_days_project(json, json_len);
	return (student);
}

void free_student(t_student *student)
{
	free(student->login);
	free(student->first_name);
	free(student->url);
	free(student->image_url);
	free(student->pool_month);
	free(student->pool_year);
	free(student->current_project);
	free(student);
}

char *student_to_json(t_student *student)
{
	char *json = NULL;

	json = mjson_aprintf(
			"{"
				"%Q:%d,"
				"%Q:%s,"
				"%Q:%s,"
				"%Q:%s,"
				"%Q:%s,"
				"%Q:%s,"
				"%Q:%s,"
				"%Q:%d,"
				"%Q:%d,"
				"%Q:%B,"
				"%Q:%s,"
				"%Q:%d"
			"}",
			"intra_id", student->intra_id,
			"login", student->login,
			"first_name", student->first_name,
			"url", student->url,
			"image_url", student->image_url,
			"pool_month", student->pool_month,
			"pool_year", student->pool_year,
			"correction_points", student->correction_points,
			"wallet", student->wallet,
			"is_staff", student->is_staff,
			"current_project", student->current_project,
			"avg_days_project", student->avg_days_project
			);
	return (json);
}
