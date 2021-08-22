#include "bson/bson.h"
#include "profiler.h"

static mongoc_collection_t *get_collection(t_mongo *mongo)
{
	return mongoc_client_get_collection(mongo->client, "intra_profiler", "students");
}

int init_db(t_mongo *mongo)
{
	const char *uri_string = getenv("MONGO_URL");

	mongoc_client_t *client;
	mongoc_database_t *database;

	client = mongoc_client_new(uri_string);
	if (!client)
	{
		fprintf(stderr, "Failed to generate a client\n");
		return (0);
	}

	mongoc_client_set_appname(client, "intra-profiler");
	database = mongoc_client_get_database(client, "intra_profiler");
	mongo->client = client;
	mongo->database = database;
	return (1);
}

void close_db(t_mongo *mongo)
{
	mongoc_database_destroy(mongo->database);
	mongoc_client_destroy(mongo->client);
	mongoc_cleanup();
}

void insert_student(t_mongo *mongo, t_student *student)
{
	bson_t *student_document;
	bson_error_t error;
	bson_oid_t oid;
	mongoc_collection_t *collection;
	char *student_json;

	student_json = student_to_json(student);
	collection = mongoc_client_get_collection(mongo->client,
			"intra_profiler", "students");

	student_document = bson_new_from_json((uint8_t *)student_json,
			strlen(student_json), &error);

	bson_oid_init(&oid, NULL);
	BSON_APPEND_OID(student_document, "_id", &oid);
	if (!mongoc_collection_insert_one(
				collection, student_document, NULL, NULL, &error)) {
		fprintf(stderr, "error: %s\n", error.message);
	}

	mongoc_collection_destroy(collection);
	bson_destroy(student_document);
	free(student_json);
}

int student_exists(t_mongo *mongo, int intra_id, bson_oid_t *oid)
{
	int exists = 0;
	const bson_t *doc;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	bson_t *query;

	query = bson_new();
	BSON_APPEND_INT32 (query, "intra_id", intra_id);

	collection = get_collection(mongo);
	cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);
	if(mongoc_cursor_next(cursor, &doc))
	{
		bson_iter_t iter;
		bson_iter_init(&iter, doc);
		while (bson_iter_next(&iter))
			if (BSON_ITER_HOLDS_OID(&iter))
				bson_oid_copy(bson_iter_oid(&iter), oid);
		exists = 1;
	}
	mongoc_collection_destroy(collection);
	bson_destroy(query);
	mongoc_cursor_destroy(cursor);
	return (exists);
}

static int update_student(t_mongo *mongo, bson_oid_t *oid, t_student *student)
{
	int success = 1;
	mongoc_collection_t *collection = NULL;
	bson_error_t error;
	bson_t update;
	bson_t *new_data = NULL;
	bson_t *query = NULL;

	char *str_student = student_to_json(student);
	new_data = bson_new_from_json((const uint8_t *) str_student, -1, &error);
	free(str_student);
	if (!new_data)
	{
		fprintf(stderr, "failed to generate document for update\n%s", error.message);
		success = 0;
	}
	else
	{
		bson_init(&update);
		bson_append_document(&update, "$set", 4, new_data);
		query = BCON_NEW("_id", BCON_OID(oid));
		collection = get_collection(mongo);

		if (!mongoc_collection_update_one(
					collection, query, &update, NULL, NULL, &error))
		{
			fprintf(stderr, "failed to update: %s\n", error.message);
			success = 0;
		}
	}
	bson_destroy(query);
	bson_destroy(new_data);
	bson_destroy(&update);
	mongoc_collection_destroy(collection);
	return (success);
}

int save_student(t_mongo *mongo, int id, t_student *student)
{
	
	bson_oid_t oid;

	if (student_exists(mongo, id, &oid))
		update_student(mongo, &oid, student);
	else
		insert_student(mongo, student);
	return 1;
}
