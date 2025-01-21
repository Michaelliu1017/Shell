# ifndef FUNCTION_H
# define FUNCTION_H

void prompt_loop();

void program_locate(char ** cup, int size);


void input_output_redirect(char ** cup, int size);

// Work in Progress
void pipeline(char ** cup, int size);

void cd_cmd(char ** cup, int size);

// Work in Progress
void jobs_cmd(char ** cup, int size);

// Work in Progress
void fg_cmd(char * index);

void exit_cmd();

# endif 