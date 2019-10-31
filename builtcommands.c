#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include "builtcommands.h"
void echo(){
     for(int i=1; i<words; i++){
          printf("%s ",inputstrings[i]);
     }
     printf("\n");
}

void cd(char *path){
     char cwd[size];
     getcwd(cwd,size);
     if(words>2){
          printf("Too many arguments in the command\n");
     }
     else{
          if(strncmp(inputstrings[1],"~",size)==0){
               chdir(path);
          }
          else if(chdir(inputstrings[1])!=0){
               chdir(cwd);
               printf("This file path does not exist\n");
          }
     }
}

void pwd(){
     char cwd[size];
     getcwd(cwd,size);
     if(words>1){
          printf("Too many arguments in the command\n");
     }
     else{
          printf("%s\n",cwd);
     }
}
