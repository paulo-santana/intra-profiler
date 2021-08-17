#ifndef PROFILER_H
# define PROFILER_H

#include "mongoose.h"
#include <stdio.h>

typedef struct s_api {
	int				keep_running;
	struct mg_mgr	mgr;
} t_api;

#endif
