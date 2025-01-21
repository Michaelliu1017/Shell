# include "function.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h> 


//***************************************************************************************** */
//       [1] Prompt                            <>
//***************************************************************************************** */
void prompt_loop() {

    char cwd[4096];  // store dir

    getcwd(cwd, sizeof(cwd));  // get dir

    char *directory = basename(cwd);  // Get the dir last part 

    printf("[nyush %s]$ ", directory);  // Print prompt
    fflush(stdout);  // Output immediately
}
//***************************************************************************************** */
//       [2] Program Locating                <ACP>                  
//***************************************************************************************** */

void program_locate(char ** cup, int size){
    char *path;

    //---- CASE 1: Absolute Path------------------------------------
    if (cup[0][0] == '/') {
        path = cup[0];
       
        //printf("[D] absolute path: %s\n", path);
        if (access(path, X_OK) == 0) {
            execv(path, cup);  
        } else {
            fprintf(stderr,"Error: invalid program\n");
        }

    }
    //---- CASE 2: Relative Path------------------------------------
    else if (strchr(cup[0], '/') != NULL) {
        path = cup[0];
         cup[size] = NULL;
        //printf("[D] relative path: %s\n", path);
        if (access(path, X_OK) == 0) {
            execv(path, cup);  
        } else {
             fprintf(stderr,"Error: invalid program\n");
        }
        
    }
    //---- CASE 3: /usr/bin ----------------------------------------
    else{
        path=malloc(10+strlen(cup[0]));
        char *def_path = "/usr/bin/";
        strcpy(path, def_path);
        strcat(path, cup[0]);

        if (access(path, X_OK) == 0) {
            execv(path, cup);              
        } else {
             fprintf(stderr,"Error: invalid program\n");
        }
        free(path);
    }
     
}
  

//***************************************************************************************** */
//       Input_Output_Redirect                           <>   
//***************************************************************************************** */
void input_output_redirect(char ** cup, int size){
    char *output_file = NULL;
    char *input_file =NULL;
    char * path[1024];
    int p_size=0;
    bool flag=false;

    for(int i=0;i<size;i++){

        if((strcmp(cup[i], ">") == 0)||(strcmp(cup[i], ">>") == 0)){
           // printf();
            if(strcmp(cup[i], ">>") == 0){
                flag=true;
            }
            i++;
            output_file = cup[i]; 

          //  continue;

        }else if(strcmp(cup[i], "<") == 0){
            i++;
            input_file=cup[i];
        }else{  
            path[p_size]=cup[i];
            p_size++;
        }
    }
    path[p_size]=NULL;
    int filed;
    if(input_file){
       // printf("NB\n");
        if(open(input_file,O_RDONLY)!=-1){
                    dup2(open(input_file, O_RDONLY), STDIN_FILENO); 
        }else{
            fprintf(stderr,"Error: invalid file\n");
            return;
        }
    }

    if (output_file) {
        if (flag) {
            filed = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644); 
        } else {
            filed = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
        }
        if (filed < 0) {
            fprintf(stderr,"Error: invalid file");
            return;
        }
        dup2(filed, STDOUT_FILENO); 
        close(filed);
    }

     execvp(path[0], path);

}

//***************************************************************************************** */
//       PIPELINE HANDLER                <>               
//***************************************************************************************** */
void pipeline(char ** cup, int size){
    printf("WORK IN PROGRESS: Pipeline");
    
}
//***************************************************************************************** */
//       CD CMD                          <F>          
//***************************************************************************************** */
void cd_cmd(char ** cup, int size){
    // !!!!! revision !!!!!
   char *path[1024];  
    int p_count =0;
    int count=1;
   
    while (cup[count]!= NULL) {
        path[p_count] = cup[count];
        p_count++;
        count++;
    }


    if (p_count == 0 || p_count>=2) {
        fprintf(stderr,"Error: invalid command\n");
        return;
    }

    
     char *dir = path[0];
     if (chdir(dir) != 0) {
         fprintf(stderr,"Error: invalid directory\n");
     }

}

//***************************************************************************************** */
//       JOBS CMD                          <>        
//***************************************************************************************** */
void jobs_cmd(char ** cup, int size){
    printf("WORK IN PROGRESS: jobs_cmd\n");
}

//***************************************************************************************** */
//       FG CMD                            <>     
//***************************************************************************************** */
void fg_cmd(char * index){
    printf("WORK IN PROGRESS: fg_cmd\n");
}

//***************************************************************************************** */
//       EXIT CMD                          <>       
//***************************************************************************************** */
void exit_cmd(){
       exit(0);  
}
