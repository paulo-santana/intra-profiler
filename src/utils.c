#include "profiler.h"

void dump_response(t_response *response)
{
	printf("\n>> response <<\n");
	printf("\n>> code: %d<<\n", response->code);
	printf("\n>> body: %s\n ^^^\n", response->body);
}
