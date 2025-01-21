#include "helper.h"
#include "function.h"
#include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h> // boolean 
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>


extern bool pipeFlag;
extern int piCount;
char ** cup;
int size=0;
int Bi_cmd_flag=-1;
int i=0;
char * fg_arg=NULL;
//bool pipeFlag=false;
//***************************************************************************************** */
//       [1] Command Handler                               
//***************************************************************************************** */
void command_hdl(char * command){
    char ***dpcup=malloc((piCount+1) * sizeof(char **));
    int size_rec[piCount+1];

    //printf("");
    if(!pipeFlag){
        command_itp(command);
    }else{
       // printf("Work in progress: command_hdl\n");
        //Preparation for pipe
        //Cut command > get data> pipe > fork > ???
        char ** subcup;
         subcup=(char **)malloc((piCount+1) * sizeof(char *));
        int cc=0;
        char * cmd_cup = strdup(command);
        cmd_cup[strcspn(cmd_cup, "\n")] = 0;
        char * str = strtok(cmd_cup, "|");
        
        while(str!=NULL){
        
             subcup[cc] = (char *)malloc((strlen(str) + 1) * sizeof(char*));
             if (subcup[cc] == NULL) {
                   fprintf(stderr,"[E] ERROR: MALLOC FAILED\n");
        }
        strcpy(subcup[cc], str); 
      //  printf("[%d]Token: %s\n",cc,str);
        cc++;
        str = strtok(NULL, "|");
        }
        

        for(int i=0;i<piCount+1;i++){
            size_rec[i]=count_size(subcup[i]);
            dpcup[i]=cupcut(subcup[i],size_rec[i]);
          // printf("DP POINTER : %p\n",dpcup[i]);
        }
     // Pipe Core
    //？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
   if (fork() == 0) {  
    int input = 0;
    int pp[2];
 //************************************************************ */
    for (int i = 0; i < piCount+1; i++) {
        
        if(i!=piCount){
         
          pipe(pp);
        }

       if(fork()==0){
        //???????????????????????????????????????????????????????
            if(i==0){
                close(pp[0]);
                dup2(pp[1],STDOUT_FILENO);
                close(pp[1]);

            }else if(i==piCount){
                dup2(input,STDIN_FILENO);
                close(input);
            }else{
                close(pp[0]);
                dup2(input,STDIN_FILENO);
                close(input);
                dup2(pp[1],STDOUT_FILENO);
                close(pp[1]);
            }
        //???????????????????????????????????????????????????????
            input_output_redirect(dpcup[i], size_rec[i]);
            exit(0);  
       }else{
   //     if (input != 0) {
        
   //         close(input);
   //     }

   //     if (i != piCount) {
            // 父进程关闭当前管道的写端
    //        close(pp[1]);
    //        input = pp[0]; 
    //    }
        //------------------------------------
        if(i==0){
            close(pp[1]);
            input = pp[0];
        }else if(i==piCount){
            close(input);
        }else{
            close(input);
            close(pp[1]);
            input = pp[0];
        }
        //-----------------------------------
       }
    
    }
    
//************************************************************ */

    for (int i = 0; i <= piCount; i++) {
        wait(NULL);
    }
    
    exit(0);  
} else {
   
    wait(NULL);
}

    //？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
    }
}

//***************************************************************************************** */
//       [2] Command Interpretor                                  
//***************************************************************************************** */
void command_itp(char *command) {
    bool input_flag = false;
    bool output_flag = false;

        //--------------- Build in Cmd Check ----------------------------/
           
             if(bi_check(command)){
            
                switch(Bi_cmd_flag){
                    case -1:
                         fprintf(stderr,"Error: invalid command\n");
                         break;
                    case 0:
                        // printf("Bi_cmd_Flag=0 \n");
                         cd_cmd(cup,size);
                         break;
                    case 1:
                       //  printf("Bi_cmd_Flag=1 \n");
                         jobs_cmd(cup,size);
                         break;
                    case 2:
                        // printf("Bi_cmd_Flag=2 \n");
                         fg_cmd(fg_arg);
                         break;
                    case 3:
                        // printf("Bi_cmd_Flag=3 \n");
                       
                         exit_cmd();
                         break;
                }
             }else{
              //------------------ Core Code -----------------------/
                
                //determine flags
                 for (int i = 0; i < size; i++) {
                    if (strcmp(cup[i], "<") == 0) {
                        input_flag = true;
                         break;
                    }

                    if (strcmp(cup[i], ">") == 0 || strcmp(cup[i], ">>") == 0) {
                         output_flag = true;
                         break;
                    }
                }
                //3 cases
                if(input_flag){
                      //printf("#INPUT DETECTED\n");
                    if(fork()==0){
                         //input_redirect(cup,size);
                         input_output_redirect(cup,size);
                         exit(0);
                    }else{
                         wait(NULL);
                       //  printf("BTP 1\n");
                    }
                }else if(output_flag){
                   // printf("#OUTPUT DETECTED\n");
                    if(fork()==0){
                   //?????????????????????????????????????????????????
                    // output_redirect(cup,size);
                   input_output_redirect(cup,size);
                   //?????????????????????????????????????????????????
                         exit(0);
                    }else{
                         wait(NULL);
                       //  printf("BTP 2\n");
                    }
                }else{
                     //printf("#PROGRAM LOC\n");
                    if(fork()==0){
                         program_locate(cup,size);
                         exit(0);
                    }else{
                         wait(NULL);
                         //printf("BTP 3\n");
                    }
                }
               
               
              //  printf("[C] WORK IN PROGRESS\n");
                for (int i = 0; i < size; i++) {
                     free(cup[i]);
                    }
                free(cup);
         
            }
}

//***************************************************************************************** */
//       [2] Command Error Checker                              
//***************************************************************************************** */
bool command_err(char *command) {

    int inputNum=0;
    int outputNum=0;
    int pipeNum=0;
   
    
    size=0;
   // bool pipeFlag=false;
    bool biFlag = false;
    //char ** cup;
    char *str;

    char *cmd_cup = strdup(command);
    cmd_cup[strcspn(cmd_cup, "\n")] = 0;
    str=strtok(cmd_cup, " ");
    
    // PreCount
     while(str!= NULL){
        
          if(strcmp(str,"|")==0){
            
            pipeFlag=true;
             pipeNum++;
        }
        if(bi_check(str)){
            biFlag=true;
        }
        if (sp_check(str)){
            // printf("[ERROR] Spelling Mistake Detected\n");
            return false;
        }
        size++;
        str = strtok(NULL, " "); 
        
    }

    //------------------------------------------------------/
    // 10/4/24 double pointer storage 
        cup=(char **)malloc((size) * sizeof(char *));
        int cc=0;
        cmd_cup = strdup(command);
        cmd_cup[strcspn(cmd_cup, "\n")] = 0;
        str = strtok(cmd_cup, " ");
        
       // printf("------------------------------\n");
        while(str!=NULL){
        
             cup[cc] = (char *)malloc((strlen(str) + 1) * sizeof(char));
             if (cup[cc] == NULL) {
                   fprintf(stderr,"[E] ERROR: MALLOC FAILED\n");
                return false;
        }
        strcpy(cup[cc], str); 
       // printf("[%d]Token: %s\n",cc,str);
        cc++;
        str = strtok(NULL, " ");
        }
        // null terminator
       // cup[size+1]="\0";
       // size++;
    //------------------------------------------------------/
    //printf("------------------------------\n");
    //printf("SIZE IS : %d\n",size);
    //free(cmd_cup); 

    //######################################################/
    //##                  Core Code                       ##/
    //######################################################/
    for(int i=0;i<size;i++){
        
      //printf("Token %d: %s\n",i,cup[i]);
       
        //_______________________________________________/
        //         Pipeline Check
        //_______________________________________________/

        //
        
        //printf("NULL is :%s\n",cup[size]);
        
        //check
        if(strcmp(cup[i],"|")==0){
            piCount++;
            //printf("INCREASED\n");
            // requirement 2

           // printf("i is : %d\n",i);
           // printf("size-1= %d\n",size-1);
           
            if(i==0||i==size-1){
              //  printf("[ERROR] Pipeline Syntax Error: | [pos %d]\n",i);
                
                return false;
            // requirement 3
            }else if(biFlag){
               // printf("[ERROR] Build-in Command Cannot be I/O Redirected or Piped: 3\n");
                return false;
            }
         }

        //_______________________________________________/
        //         Output redirection 
        //_______________________________________________/
         if(strcmp(cup[i],">")==0||strcmp(cup[i],">>")==0){

            // printf("[T] inside [out] \n");
            outputNum++;
            // requirement 2
            if(outputNum>1){
              //  printf("[ERROR] OutputNum Exceed Requirement: > or >>\n");
                return false;
            // requirement 3
            }else if(biFlag){
              //  printf("[ERROR] Build-in Command Cannot be I/O Redirected or Piped: 1\n");
                return false;
            // requirement 1
            }else if(pipeFlag && piCount!=pipeNum){
              // printf("PiCount: %d\n",piCount);
              //  printf("PipeNum: %d\n",pipeNum);
              //  printf("[ERROR] Output Redirection Placed in Wrong Position: Pipe\n");
                return false;
            }else if(i==0||i==size-1){
             //   printf("[ERROR] Output Redirection Placed in Wrong Position: Head or Tail (pos: %d)\n",i);
                return false;
            }
            //?????????????????????????????????????????????
            if(i + 1 < size && i + 2 < size){
             //   printf("[T] Inner Check Begin\n");
              //  printf("Next token 1: %s\n", cup[i+1]);
             // printf("Next token 2: %s\n", cup[i+2]);
                
                if (strcmp(cup[i+2], "<") != 0 && strcmp(cup[i+2], "|") != 0) {
                //    printf("[ERROR] Input redirection cannot have multiple files.\n");
                    return false;
                  }
         
                } 
            //?????????????????????????????????????????????
         }

        //_______________________________________________/
        //         Input redirection 
        //_______________________________________________/
         if(strcmp(cup[i],"<")==0){
           // printf("[T] inside [in] \n");
            inputNum++;
            // requirement 2
            if(inputNum>1){
             //   printf("[ERROR] InputNum Exceed Requirement: <\n");
                return false;
            // requirement 3
            }else if(biFlag){
              //  printf("[ERROR] Build-in Command Cannot be I/O Redirected or Piped: 2\n");
                return false;
            // requirement 1
            }else if(pipeFlag&&piCount!=0){
              //  printf("[ERROR] Input Redirection Placed in Wrong Position\n");
                return false;
            }else if(i==0||i==size-1){
             //   printf("[ERROR] Input Redirection Placed in Wrong Position: Head or Tail (pos: %d)\n",i);
                return false;
            }
            //?????????????????????????????????????????????
            if(i + 1 < size && i + 2 < size){
             //   printf("[T] Inner Check Begin\n");
              //  printf("Next token 1: %s\n", cup[i+1]);
              //  printf("Next token 2: %s\n", cup[i+2]);
                
                if (strcmp(cup[i+2], ">") != 0 && strcmp(cup[i+2], ">>") != 0 && strcmp(cup[i+2], "|") != 0) {
              //      printf("[ERROR] Output redirection cannot have multiple files.\n");
                    return false;
                  }
         
                } 
            //?????????????????????????????????????????????
         }

    }
    // Clean Mem
    //for (int i = 0; i < size; i++) {
    //    free(cup[i]);
   // }
    //free(cup);
    
   
    return true;
}
// end line 

//***************************************************************************************** */
//       [3] Build-in Command Checker     <ACP>                         
//***************************************************************************************** */
bool bi_check(char * command){
  
    char_cln(command);

    if (strncmp(command, "cd",2) == 0) {
        Bi_cmd_flag=0;
        return true;
    } else if (strncmp(command, "jobs",4) == 0) {
        if(size>1){
           // printf("HAS TOKEN SIZE IS : %d\n",size);
            Bi_cmd_flag= -1;
           // printf("FLAG: %d\n",Bi_cmd_flag);
            return true;

        }else{
             Bi_cmd_flag=1;
             return true;
        }
    }else if(strncmp(command, "fg",2) == 0){
        //???????????????????????????????????????????????????
       
         int count=-1;
        char * token;
        char *cmd_cup = strdup(command);
        cmd_cup[strcspn(cmd_cup, "\n")] = 0;
        token=strtok(cmd_cup, " ");
  
        
        while((token=strtok(NULL, " "))!=NULL){
             count++;
            if(count==0){
                fg_arg=token;
                Bi_cmd_flag=2;
                // 2+ args
            }else if(count>0){
                 fg_arg=NULL;
                 Bi_cmd_flag=-1;
                 free(cmd_cup);
                 return true;
            }
            
            
        }
        // 0 arg
        if(count==-1){
            fg_arg=NULL;
            Bi_cmd_flag=-1;
            free(cmd_cup);
            return true;
        }
        
         printf("%s\n",fg_arg);
       
        free(cmd_cup);
            return true;
        
        //???????????????????????????????????????????????????
    } else if (strncmp(command, "exit",4) == 0) {
       
        if(size>1){
           // printf("HAS TOKEN SIZE IS : %d\n",size);
            Bi_cmd_flag= -1;
           // printf("FLAG: %d\n",Bi_cmd_flag);
            return true;

        }else{
        Bi_cmd_flag=3;
        // printf("3FLAG: %d\n",Bi_cmd_flag);
        return true;
        }
    } else {
        Bi_cmd_flag=-1;
        return false;  
    }
}

//***************************************************************************************** */
//       [4] Spelling Error Checker                              
//***************************************************************************************** */
bool sp_check(char * command){

    if(strcmp(command,"<<")==0){
        return true;
    }
    return false;
    
}

//***************************************************************************************** */
//       [6] Signal Handler          <ACP>                    
//***************************************************************************************** */
void sigint_handler(int sig){
       if ((sig ==SIGINT) || (sig==SIGQUIT)||(sig=SIGTSTP)){
        printf("\n");
        prompt_loop();
       }    
}

//***************************************************************************************** */
//       [7] Char Space & \0 Cleaner    <F>                          
//***************************************************************************************** */
void char_cln(char *str) {
    int len = strlen(str);
    //cptest
    if(len>0){
        while(isspace((unsigned char)str[len - 1])){
        len = len - 1;
        str[len] = '\0';
        }
    }
}

char ** cupcut(char * command,int subsize){
    char ** subcup;
    char * cmd_cup;
    char *str;
    

     subcup=(char **)malloc((subsize) * sizeof(char *));
        int cc=0;
        cmd_cup = strdup(command);
        cmd_cup[strcspn(cmd_cup, "\n")] = 0;
        str = strtok(cmd_cup, " ");
        
       // printf("------------------------------\n");
        while(str!=NULL){
        
             subcup[cc] = (char *)malloc((strlen(str) + 1) * sizeof(char));
             if (subcup[cc] == NULL) {
                   fprintf(stderr,"[E] ERROR: MALLOC FAILED\n");
                return false;
        }
        strcpy(subcup[cc], str); 
       // printf("[%d]Sub Token: %s\n",cc,str);
        cc++;
        str = strtok(NULL, " ");
        }

        return subcup;
}

int count_size(char * command){
    int subsize=0;
    char *cmd_cup = strdup(command);
    cmd_cup[strcspn(cmd_cup, "\n")] = 0;
    char * str=strtok(cmd_cup, " ");

     while(str!= NULL){
        subsize++;
        str = strtok(NULL, " "); 
    }
    return subsize;
}