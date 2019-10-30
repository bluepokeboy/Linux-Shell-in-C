#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<errno.h>
extern int size;
extern int words;
extern char *inputstrings[200];
void pinfo(char * path){
     int state;
     int memoryusage;
     int fl=0;
     char procfile[size];
     char symlink[size];
     strcpy(procfile,"/proc/");
     strcpy(symlink,"/proc/");
     if(words==1){
          char tempstr[size];
          int procid=getpid();
          snprintf(tempstr,size,"%d",procid);
          strcat(procfile,tempstr);
          strcat(symlink,tempstr);
          //itoa(procid,tempstr,10)
          printf("pid -- %d\n", procid);
          strcat(procfile, "/stat");
          strcat(symlink, "/exe");
          fl=1;
     }
     if(words==2){
          strcat(procfile,inputstrings[1]);
          strcat(symlink,inputstrings[1]);
          //printf("%s\n",procfile);
          struct stat fileStat;
          if(stat(procfile, &fileStat) == -1 && errno == ENOENT){
              printf("There is no process with the input pid\n");
              return;
          }
          printf("pid -- %s\n", inputstrings[1]);
          strcat(procfile, "/stat");
          strcat(symlink, "/exe");
          fl=1;
     }
     if(fl==1){
          FILE *file = fopen(procfile, "r");
          char tokenpid[1024];
          fgets(tokenpid, 1024, file);
          char *info = strtok(tokenpid, " ");
          int count = 1;
          while (count < 3 && info != NULL){
               info = strtok(NULL, " ");
               count++;
          }

          state = info[0];

          while(count < 23 && info != NULL){
               info = strtok(NULL, " ");
               count++;
          }
          memoryusage = atoi(info);
          fclose(file);
          printf("Process Status -- %c\n", state);
          printf("memory -- %d {Virtual Memory}\n", memoryusage);
          char *exec;
          exec = (char *)malloc(200);
          for (int i = 0; i < 200; i++)
              exec[i] = '\0';
          readlink(symlink, exec, 200);
          printf("%s\n", exec);
          return;
     }
     else{
          printf("Valid number of arguments have not been given\n");
     }
}
