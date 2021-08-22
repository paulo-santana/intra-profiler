#include "profiler.h"
#include "mjson.h"
#include <stdlib.h>
#include <time.h>

#define TOKEN_LEN 64

void get_token(t_api *api)
{
	if (api->token.str != NULL && api->token.expiration_date > time(NULL))
		return ;
	char *client_id = getenv("INTRA_CLIENT_ID");
	char *client_secret = getenv("INTRA_SECRET");
	char *auth_url = "https://api.intra.42.fr/oauth/token";

	char data[184];

	snprintf(data, 184,
			"grant_type=client_credentials&"
			"client_id=%s&"
			"client_secret=%s",
			client_id, client_secret);
	t_response *response = request_intra(api, "POST", auth_url, data);
	if (response->code == 200)
	{
		double expires_in;
		char *token = malloc(TOKEN_LEN + 1);
		int result = mjson_get_string(response->body, response->body_len,
				"$.access_token", token, TOKEN_LEN + 1);
		if (result < 0)
			exit (EXIT_FAILURE + 50);
		api->token.str = token;
		mjson_get_number(response->body, response->body_len,
				"$.expires_in", &expires_in);
		api->token.expiration_date = time(NULL) + expires_in;
	}
	else
	{
		api->token.str = NULL;
		api->token.expiration_date = 0;
	}
	free_response(response);
}
