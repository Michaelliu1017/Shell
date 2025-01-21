#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>

//unfinished part
void command_hdl(char * command);

void command_itp(char *command);

bool command_err(char *command);

bool bi_check(char *command);

bool sp_check(char * command);

void sigint_handler(int sig);

void char_cln(char * str);

char ** cupcut(char *command ,int subsize);

int count_size(char * command);
#endif