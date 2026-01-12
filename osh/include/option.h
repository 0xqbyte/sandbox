#ifndef OPTION_H
#define OPTION_H

#include "context.h"

#define OPT_CONCURRENT "&"
#define OPT_OUTPUT_REDIRECT ">"
#define OPT_INPUT_REDIRECT "<"
#define OPT_PIPE "|"

bool has_opt_concurent(Context *ctx);
int handle_opt_output_redirect(Context *ctx);
int handle_opt_input_redirect(Context *ctx);
int has_opt_pipe(Context *ctx);

#endif