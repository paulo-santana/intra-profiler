#include "profiler.h"
#include "mjson.h"
#include <stdlib.h>
#include <time.h>

#define TOKEN_LEN 64

void get_token(t_api *api)
{
	if (api->token.str != NULL && api->token.expiration_date < time(NULL))
	{
		printf("no need to request a token :)\n");
		return ;
	}
	char *client_id = "87cbc2db2e4d3605e36af49df3b34b8311500fb8f106307569abd91122a32276";
	char *client_secret = getenv("INTRA_SECRET");
	char *auth_url = "https://api.intra.42.fr/oauth/token";

	char data[184];

	snprintf(data, 184,
			"grant_type=client_credentials&client_id=%s&client_secret=%s",
			client_id, client_secret);
	t_response *response = request_intra(api, "POST", auth_url, data);
	dump_response(response);
	if (response->code == 200)
	{
		double expires_in, created_at;
		char *token = malloc(TOKEN_LEN + 1);
		int result = mjson_get_string(response->body, response->body_len,
				"$.access_token", token, TOKEN_LEN + 1);
		if (result < 0)
			exit (EXIT_FAILURE + 50);
		api->token.str = token;
		mjson_get_number(response->body, response->body_len,
				"$.expires_in", &expires_in);
		mjson_get_number(response->body, response->body_len,
				"$.created_at", &created_at);
		api->token.expiration_date = expires_in + created_at;
	}
	else
		api->token.str = NULL;
}
