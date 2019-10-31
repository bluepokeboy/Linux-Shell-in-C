#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pwd.h>
#include<unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
extern char username[200];
extern char systemname[200];
#define BBLUE "\x1B[1;34m"
#define RESET "\x1B[0m"
void currentadress(char *path, int lenpath, int size){
     char cwd[size];
     getcwd(cwd,size);
     if(strncmp(cwd,path,lenpath)==0){
          printf( BBLUE "<%s@%s:~%.*s>" RESET ,username,systemname,size,cwd+lenpath);
     }
     else{
          printf( BBLUE "<%s@%s:%s>" RESET ,username,systemname,cwd);
     }
}

char* namefrompath(char* path){
     for(int i = strlen(path) - 1; i; i--){
          if (path[i] == '/'){
               return &path[i+1];
          }
      }
      return path;
}
