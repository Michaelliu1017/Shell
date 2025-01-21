#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>  
#include <string.h>  
#include <stdlib.h>  
#include "function.h"
#include "helper.h"
#include <signal.h>


 bool pipeFlag=false;
 int piCount=0;

int main() {

    signal(SIGINT, sigint_handler); 
    signal(SIGQUIT, sigint_handler); 
    signal(SIGTSTP, sigint_handler); 
    while (1) {
       
        prompt_loop();  
        
        char command[1000];

         if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("exit\n");
            exit(0);
        }
           
        if(!command_err(command)){
               fprintf(stderr,"Error: invalid command\n");
        }else{
        command_hdl(command);
        }
       pipeFlag=false;
       piCount=0;
    }
    return 0;
    
}
