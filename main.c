#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include<sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include<errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include"input.h"
#include"address.h"
#include"builtcommands.h"
#include"ls.h"
#include"pinfo.h"
int size=200;
int jobsnum=0;
int running[200];
int pids[200];
char *jobs[200];
char *commands[200];
char username[200];
char systemname[200];
char *inputstrings[200];
char *inputredstrings[200];
char *outputredstrings[200];
char *doutputredstrings[200];
char *pipestrings[200];
int pidstack[200];
char processstack[1000][200];
int pidtop=0;
int numcom=0;
int words=0;
int pipes=0;
int inp=0;
int out=0;
int dout=0;
int exitf=1;
char processStack[1000][200];
int pidStack[200];
int pidTop=0;
int fgprocid=-1;
void procExit()
{
     int status;

     for (int i = 0; i < pidTop; i++)
     {
          if (waitpid(pidStack[i], &status, WNOHANG) > 0)
          {
               if (WIFEXITED(status) > 0){
                    printf("%s with pid %d exited normally\n", processStack[i], pidStack[i]);
                    running[i]=0;
               }
               else if (WIFSIGNALED(status)){
                    printf("%s with pid %d exited with signal\n", processStack[i], pidStack[i]);
                    running[i]=0;
               }
               else{
                    printf("%s with pid %d exited abnormally\n", processStack[i], pidStack[i]);
                    running[i]=0;
               }
          }
     }
}

void handler(int x){
     if(fgprocid!=-1){
          kill(fgprocid,x);
     }
}
void execute(char * path,int flag, int savestdout){
     signal(SIGCHLD, procExit);
     signal(SIGINT, handler);
     signal(SIGTSTP, handler);
     inputstrings[words]=NULL;
     if(strncmp(inputstrings[0],"echo",size)==0){
          echo();
     }
     else if(strncmp(inputstrings[0],"cd", size)==0){
          cd(path);
     }
     else if(strncmp(inputstrings[0],"pwd",size)==0){
          pwd();
     }
     else if(strncmp(inputstrings[0],"ls",size)==0){
          ls(path);
     }
     else if(strncmp(inputstrings[0],"pinfo",size)==0){
          pinfo(path);
     }
     else if(strncmp(inputstrings[0],"quit",size)==0){
          exitf=0;
     }
     else if(strncmp(inputstrings[0],"history",size)==0){
          if(words==1){
               if(pidtop-10>0){
                    for(int i=pidtop-10; i<pidtop; i++){
                         printf("%s\n", processstack[i]);
                    }
               }
               else{
                    for(int i=0; i<pidtop; i++){
                         printf("%s\n", processstack[i]);
                    }
               }
          }
          if(words==2){
               int num;
               num=atoi(inputstrings[1]);
               if(pidtop-num>0){
                    for(int i=pidtop-num; i<pidtop; i++){
                         printf("%s\n", processstack[i]);
                    }
               }
               else{
                    for(int i=0; i<pidtop; i++){
                         printf("%s\n", processstack[i]);
                    }
               }
          }
     }
     else if(strncmp(inputstrings[0],"setenv",size)==0){
          if(words==2){
               if(setenv(inputstrings[1],"",1)<0){
                    perror("Error");
                    return;
               }
          }
          else if(words==3){
               if(setenv(inputstrings[1],inputstrings[2],1)<0){
                    perror("Error");
                    return;
               }
          }
          else{
               printf("Incorrect number of arguments\n");
               return;
          }
     }
     else if(strncmp(inputstrings[0],"unsetenv",size)==0){
          if(words==2){
               if(unsetenv(inputstrings[1])<0){
                    perror("Error");
                    return;
               }
          }
          else{
               printf("Incorrect number of arguments\n");
               return;
          }
     }
     else if(strncmp(inputstrings[0],"jobs",size)==0){
          if(words==1){
               for(int i=0; i<pidTop; i++){
                    if(running[i]!=0){
                         char state2;
                         char procfile[size];
                         char tempstr[size];
                         strcpy(procfile,"/proc/");
                         int procid=pidStack[i];
                         snprintf(tempstr,size,"%d",procid);
                         strcat(procfile,tempstr);
                         struct stat fileStat;
                         if(stat(procfile, &fileStat) == -1 && errno == ENOENT){
                             printf("There is no process with the input pid\n");
                             return;
                         }
                         strcat(procfile, "/stat");
                         FILE *file = fopen(procfile, "r");
                         char tokenpid[1024];
                         fgets(tokenpid, 1024, file);
                         char *info = strtok(tokenpid, " ");
                         int count = 1;
                         while (count < 3 && info != NULL){
                              info = strtok(NULL, " ");
                              count++;
                         }
                         state2 = info[0];
                         if(state2=='R' || state2=='S'){
                              dprintf(savestdout,"[%d] Running %s [%d]\n",i+1,processStack[i],pidStack[i]);
                         }
                         else if(state2=='T'){
                              dprintf(savestdout,"[%d] Stopped %s [%d]\n",i+1,processStack[i],pidStack[i]);
                         }
                    }
               }
          }
          else{
               dprintf(savestdout,"Incorrect number of arguments printed\n");
          }
     }
     else if(strncmp(inputstrings[0],"kjob",size)==0){
          if(words==3){
               int jobsig=atoi(inputstrings[1]);
               if(jobsig<=pidTop){
                    int pidsig=pidStack[jobsig-1];
                    int signum=atoi(inputstrings[2]);
                    kill(pidsig,signum);
               }
               else{
                    dprintf(savestdout,"This is not a valid job number\n");
               }
          }
          else{
               dprintf(savestdout,"This is not a valid number of arguments\n");
          }
     }
     else if(strncmp(inputstrings[0],"overkill",size)==0){
          for(int i=0; i<pidTop; i++){
               if(running[i]!=0){
                    kill(pidStack[i],SIGKILL);
                    running[i]=0;
               }
          }
     }
     else if(strncmp(inputstrings[0],"fg",size)==0){
          if(words==2){
               int status2;
               int jobsig=atoi(inputstrings[1]);
               if(jobsig<=pidTop){
                    int pidsig=pidStack[jobsig-1];
                    char processfg[200];
                    strcpy(processfg,processStack[jobsig-1]);
                    kill(pidsig,SIGCONT);
                    running[jobsig-1]=0;
                    fgprocid=pidsig;
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTTOU, SIG_IGN);
                    tcsetpgrp(STDIN_FILENO,pidsig);
                    waitpid(pidsig,&status2,WUNTRACED);
                    fgprocid=-1;
                    if(WIFSTOPPED(status2)){
                         pidStack[pidTop]=(int)pidsig;
                         strcpy(processStack[pidTop],processfg);
                         running[pidTop]=1;
                         pidTop++;
                    }
                    tcsetpgrp(STDIN_FILENO,getpgrp());
                    signal(SIGTTIN, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
                    signal(SIGCHLD,procExit);
               }
               else{
                    dprintf(savestdout,"This is not a valid job number\n");
               }
          }
          else{
               dprintf(savestdout,"This is not a valid number of arguments\n");
          }
     }
     else if(strncmp(inputstrings[0],"bg",size)==0){
          if(words==2){
               int jobsig=atoi(inputstrings[1]);
               if(jobsig<=pidTop){
                    int pidsig=pidStack[jobsig-1];
                    char processfg[200];
                    strcpy(processfg,processStack[jobsig-1]);
                    kill(pidsig,SIGCONT);
               }
               else{
                    dprintf(savestdout,"This is not a valid job number\n");
               }
          }
          else{
               dprintf(savestdout,"This is not a valid number of arguments\n");
          }
     }
     else if(strncmp(inputstrings[0],"cronjob",size)==0){
          char *cronstrings[200];
          int cind=-1;
          int tind=-1;
          int pind=-1;
          for(int i=0; i<words; i++){
               if(strncmp(inputstrings[i],"-c",2)==0){
                    cind=i;
               }
               if(strncmp(inputstrings[i],"-t",2)==0){
                    tind=i;
               }
               if(strncmp(inputstrings[i],"-p",2)==0){
                    pind=i;
               }
          }
          int countercron=0;
          for(int i=cind+1; i<tind; i++){
               cronstrings[countercron]=inputstrings[i];
               countercron++;
          }
          for(int i=0; i<countercron; i++){
               inputstrings[i]=cronstrings[i];
          }
          if(strncmp(inputstrings[0],"ls",size)!=0 && strncmp(inputstrings[0],"cd",size)!=0 && strncmp(inputstrings[0],"pwd",size)!=0 && strncmp(inputstrings[0],"echo",size)!=0 && strncmp(inputstrings[0],"pinfo",size)!=0 && strncmp(inputstrings[0],"exit",size)!=0 && strncmp(inputstrings[0],"history",size)!=0 && strncmp(inputstrings[0],"setenv",size)!=0 && strncmp(inputstrings[0],"unsetenv",size)!=0 && strncmp(inputstrings[0],"jobs",size)!=0 && strncmp(inputstrings[0],"kjob",size)!=0 && strncmp(inputstrings[0],"overkill",size)!=0 && strncmp(inputstrings[0],"fg",size)!=0 && strncmp(inputstrings[0],"bg",size)!=0){
               flag=1;
          }
          int period=atoi(inputstrings[tind+1]);
          int total=atoi(inputstrings[pind+1]);
          pid_t pidcron;
          pidcron=fork();
          if(pidcron==0){
               for(int i=0; i<(int)(total/period); i++){
                    sleep(period);
                    words=countercron;
                    execute(path,flag,savestdout);
               }
               abort();
          }
     }
     else{
          int status;
          char *process=inputstrings[0];




          pid_t pid = fork();
          if (pid == 0){
               setpgid(0,0);
               if(execvp(process, inputstrings)<0)
                    dprintf(savestdout, "Error in forking\n");

          }

          else if (pid > 0)
          {
               if (!flag){
                    fgprocid=pid;
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTTOU, SIG_IGN);
                    tcsetpgrp(STDIN_FILENO,pid);
                    waitpid(pid, &status, WUNTRACED);
                    fgprocid=-1;
                    if(WIFSTOPPED(status)){
                         pidStack[pidTop]=(int)pid;
                         strcpy(processStack[pidTop],process);
                         running[pidTop]=1;
                         pidTop++;
                    }
                    tcsetpgrp(STDIN_FILENO,getpgrp());
                    signal(SIGTTIN, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
               }
               else{
                    pidStack[pidTop]=(int)pid;
                    strcpy(processStack[pidTop],process);
                    running[pidTop]=1;
                    pidTop++;
               }
          }

          else
          printf("fork error\n");
     }

     return;
}

void execpipe(char * path, int flag, int savestdout, int numpipe, int l){
     pid_t pid2;
     int pd[2];
     pipe(pd);
     pid2=fork();
     if (pid2==0) {




          if(numpipe==0){
               pid_t pidlol;
               if(l>0){
                    int fd0;
                         if ((fd0 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
                              perror("Couldn't open input file");
                              exit(0);
                         }
                         words-=2;
                         dup2(fd0, 0);
                         close(fd0);
               }
               execute(path,flag,savestdout);
               abort();



          }



          execute(path,flag,savestdout);






          abort();
     }
     else if (pid2<0)
     dprintf(savestdout,"error\n");
     else{
          int status;
          waitpid(pid2,&status,0);
          dup2(pd[0],0);
          close(pd[1]);
     }
     close(pd[0]);
     close(pd[1]);
}

int main(void){
     int savestdin=dup(0), savestdout=dup(1);
     char currentdir[]="./";
     char *histpath = realpath(currentdir, NULL);
     char *path = realpath(currentdir, NULL);
     strcat(histpath,"/history.txt");
     int lenpath=strlen(path);
     getlogin_r(username,size);
     gethostname(systemname,size);
     int fd= open(histpath, O_RDWR,O_CREAT);
     struct stat hist;
     FILE *fptr = NULL;
     if (!stat(histpath, &hist)){
          fptr = fopen(histpath, "r");
          while(fgets(processstack[pidtop], 1000, fptr)){
               processstack[pidtop][strlen(processstack[pidtop]) - 1] = '\0';
               pidtop++;
          }
     }
     else{
          creat(histpath, 0777);
     }
     int mainpid=getpid();
     while(exitf){
          signal(SIGCHLD, procExit);
          signal(SIGINT, handler);
          signal(SIGTSTP, handler);
          currentadress(path,lenpath,size);
          char *inpt;
          inpt = readline("");
          add_history(inpt);
          strcpy(processstack[pidtop++],inpt);
          if(pidtop-20>0){
               FILE *fptr2;
               fptr2 = fopen(histpath, "w");
               if(fptr2 == NULL){
                    printf("Error\n");
               }
               else{
                    for(int i=pidtop-20; i<pidtop; i++){
                         fprintf(fptr2,"%s\n", processstack[i]);
                    }
                    fclose(fptr2);
               }
          }
          else{
               FILE *fptr2;
               fptr2 = fopen(histpath, "w");
               if(fptr2 == NULL){
                    printf("Error\n");
               }
               else{
                    for(int i=0; i<pidtop; i++){
                         fprintf(fptr2,"%s\n", processstack[i]);
                    }
                    fclose(fptr2);
               }
          }
          inputsplit(inpt);
          int comnum=0;
          while(numcom){
               signal(SIGCHLD, procExit);
               signal(SIGINT, handler);
               signal(SIGTSTP, handler);
               int flag=0;
               if(commands[comnum][strlen(commands[comnum])-1] == '&' ){
                    flag=1;
                    commands[comnum][strlen(commands[comnum])-1] = '\0';
               }
               pipesplit(commands[comnum]);
               int numpipe=0, vartest=0, checkpipe=pipes;
               int countazo=0;
               while(pipes){
                    signal(SIGCHLD, procExit);
                    signal(SIGINT, handler);
                    signal(SIGTSTP, handler);
                    char *tempstring;
                    tempstring=(char*)malloc(1024);
                    char *tempstring2;
                    tempstring2=(char*)malloc(1024);
                    char *tempstring3;
                    tempstring3=(char*)malloc(1024);
                    char *tempstring4;
                    tempstring4=(char*)malloc(1024);


                    strcpy(tempstring,pipestrings[numpipe]);
                    strcpy(tempstring2,pipestrings[numpipe]);
                    strcpy(tempstring3,pipestrings[numpipe]);
                    strcpy(tempstring4,pipestrings[numpipe]);

                    inputredsplit(tempstring);
                    outputredsplit(tempstring2);
                    commandsplit(tempstring3);
                    doutputredsplit(tempstring4);
                    int l=0;
                    int r=0;
                    int inpind=-1;
                    int outind=-1;
                    int doutind=-1;
                    int flagout=0;
                    for(int i=0; i<strlen(pipestrings[numpipe]); i++){
                         if(i<strlen(pipestrings[numpipe])-1){
                              if(pipestrings[numpipe][i]=='>' && pipestrings[numpipe][i+1]=='>'){
                                   r=2;
                                   doutind=i;
                                   flagout=1;
                              }
                         }
                         if(pipestrings[numpipe][i]=='<'){
                              l=1;
                              inpind=i;
                         }
                         if(pipestrings[numpipe][i]=='>' && flagout==0){
                              r=1;
                              outind=i;
                         }
                    }
                    if(pipes>1){
                         countazo++;
                         execpipe(path, flag,  savestdout, numpipe,l);
                    }
                    else{
                         int flagazo=0;
                         dup2(savestdout, 1);
                         if(l>0 && r==0){
                              int fd0;
                                   if ((fd0 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
                                        perror("Couldn't open input file");
                                        exit(0);
                                   }
                                   words-=2;
                                   dup2(fd0, 0);
                                   close(fd0);
                         }
                         if(l==0 && r==1){
                              int fd1 ;
                              if ((fd1 = creat( inputstrings[words-1], 0644)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              words-=2;
                              close(fd1);
                         }

                         if(l==0 && r==2){
                              int fd1 ;
                              if ((fd1 = open(inputstrings[words-1],O_APPEND|O_CREAT|O_WRONLY, 0666)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              words-=2;
                              close(fd1);
                         }
                         if(l==1 && r==1){
                              int fd0;
                              if ((fd0 = open(inputstrings[words-3], O_RDONLY, 0)) < 0) {
                                   perror("Couldn't open input file");
                                   exit(0);
                              }
                              dup2(fd0, 0);
                              close(fd0);
                              int fd1 ;
                              if ((fd1 = creat( inputstrings[words-1], 0644)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              close(fd1);
                              words-=4;
                         }
                         if(l==1 && r==2){
                              int fd0;
                              if ((fd0 = open(inputstrings[words-3], O_RDONLY, 0)) < 0) {
                                   perror("Couldn't open input file");
                                   exit(0);
                              }
                              dup2(fd0, 0);
                              close(fd0);
                              int fd1 ;
                              if ((fd1 = open(inputstrings[words-1],O_APPEND|O_CREAT|O_WRONLY, 0666)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              close(fd1);
                              words-=4;
                         }
                         execute(path,flag, savestdout);
                    }
                    pipes--;
                    numpipe++;
                    words=0;
                    inp=0;
                    out=0;
               }
               dup2(savestdin, 0);
               dup2(savestdout, 1);
               comnum++;
               numcom--;
          }
          free(inpt);
     }
     return 0;
}
