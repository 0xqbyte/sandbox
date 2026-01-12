#ifndef COMMAND_H
#define COMMAND_H

#include "context.h"

#define CMD_EXIT "exit"
#define CMD_HISTORY "history"
#define CMD_LAST "!!"

bool is_cmd_exit(Context *ctx);
bool is_cmd_history(Context *ctx);
bool is_cmd_last(Context *ctx);

void cmd_history(Context *ctx);
bool cmd_last(Context *ctx);

int append_history(Context *ctx);

#endif