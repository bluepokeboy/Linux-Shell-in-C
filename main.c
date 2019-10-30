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
// char process[200][200];
// char flags[200][200];
// char args[200][200];
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
//int runni
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
// void kjobhandler(int x){
//      if(pidsig!=-1){
//           kill(pidsig,x);
//      }
// }
void execute(char * path,int flag, int savestdout){
     // dprintf(savestdout,"dasdkas\n");
     // int curr_size = 0;
     // while (inputstrings[curr_size]!="\0")
     // {
     //      curr_size++;
     // }
     // if (words<curr_size)
     //      inputstrings[words]="\0";
     //      dprintf(savestdout,"dasdkas\n");
     signal(SIGCHLD, procExit);
     signal(SIGINT, handler);
     signal(SIGTSTP, handler);
     inputstrings[words]=NULL;
     //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
     if(strncmp(inputstrings[0],"echo",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          echo();
     }
     else if(strncmp(inputstrings[0],"cd", size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          cd(path);
     }
     else if(strncmp(inputstrings[0],"pwd",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          pwd();
     }
     else if(strncmp(inputstrings[0],"ls",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          ls(path);
     }
     else if(strncmp(inputstrings[0],"pinfo",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          pinfo(path);
          //printf("%s\n",procfile);
     }
     else if(strncmp(inputstrings[0],"quit",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          exitf=0;
     }
     else if(strncmp(inputstrings[0],"history",size)==0){
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
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
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
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
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
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
          //dprintf(savestdout, "input string is %s\n", inputstrings[0]);
          //dprintf(savestdout,"%d\n",jobsnum);
          if(words==1){
               for(int i=0; i<pidTop; i++){
                    //dprintf(savestdout,"%d %s %d\n",pidStack[i],processStack[i],running[i]);
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
                         // char awake[]="R";
                         // char awake2[]="S";
                         // char dead[]="T";
                         if(state2=='R' || state2=='S'){
                              dprintf(savestdout,"[%d] Running %s [%d]\n",i+1,processStack[i],pidStack[i]);
                         }
                         else if(state2=='T'){
                              dprintf(savestdout,"[%d] Stopped %s [%d]\n",i+1,processStack[i],pidStack[i]);
                         }
                         //printf("%c\n",state);
                    }
               }
          }
          else{
               dprintf(savestdout,"Incorrect number of arguments printed\n");
          }
     }
     else if(strncmp(inputstrings[0],"kjob",size)==0){
          if(words==3){
               //dprintf(savestdout,"lol1\n");
               int jobsig=atoi(inputstrings[1]);
               if(jobsig<=pidTop){
                    //dprintf(savestdout,"lol2\n");
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
                    //add one more if for error handling
                    //int signum=atoi(inputstrings[2]);
                    kill(pidsig,SIGCONT);
                    running[jobsig-1]=0;
                    //dprintf(savestdout,"lmao1\n");
                    fgprocid=pidsig;
                    signal(SIGTTIN, SIG_IGN);
                    signal(SIGTTOU, SIG_IGN);
                    tcsetpgrp(STDIN_FILENO,pidsig);
                    waitpid(pidsig,&status2,WUNTRACED);
                    fgprocid=-1;
                    //dprintf(savestdout,"lmao2\n");
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
                    //add one more if for error handling
                    //int signum=atoi(inputstrings[2]);
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
          //printf("lol1\n");
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
          //printf("lol2\n");
          int countercron=0;
          for(int i=cind+1; i<tind; i++){
               cronstrings[countercron]=inputstrings[i];
               countercron++;
          }
          //printf("%d\n",countercron);
          for(int i=0; i<countercron; i++){
               inputstrings[i]=cronstrings[i];
          }
          if(strncmp(inputstrings[0],"ls",size)!=0 && strncmp(inputstrings[0],"cd",size)!=0 && strncmp(inputstrings[0],"pwd",size)!=0 && strncmp(inputstrings[0],"echo",size)!=0 && strncmp(inputstrings[0],"pinfo",size)!=0 && strncmp(inputstrings[0],"exit",size)!=0 && strncmp(inputstrings[0],"history",size)!=0 && strncmp(inputstrings[0],"setenv",size)!=0 && strncmp(inputstrings[0],"unsetenv",size)!=0 && strncmp(inputstrings[0],"jobs",size)!=0 && strncmp(inputstrings[0],"kjob",size)!=0 && strncmp(inputstrings[0],"overkill",size)!=0 && strncmp(inputstrings[0],"fg",size)!=0 && strncmp(inputstrings[0],"bg",size)!=0){
               //strcat(inputstrings[countercron-1],"&");
               flag=1;
          }
          //printf("lol3\n");
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
          //printf("lol4\n");
     }
     else{
          int status;
          char *process=inputstrings[0];
          //char *args[10];
          //args[0]=inputstrings[0];
          // for(int i=1; i<words; i++){
          //      args[i]=inputstrings[i];
          // }
          // inputstrings[words][0]=

          //int p1[2], p2[2];
          //pipe(p1);



          pid_t pid = fork();
          if (pid == 0){
               // if(strcmp(process,"vim")!=0 && strcmp(process,"more")!=0 && strcmp(process,"less")!=0 && strcmp(process,"man")!=0){
               //      setpgid(0,0);
               //      if(execvp(process, inputstrings)<0)
               //      dprintf(savestdout, "Error in forking\n");
               // }
               // else{
               //      if(flag==1){
               //           setpgid(0,0);
               //           if(execvp(process, inputstrings)<0)
               //           dprintf(savestdout, "Error in forking\n");
               //      }
               //      else{
               //           if(execvp(process, inputstrings)<0)
               //           dprintf(savestdout, "Error in forking\n");
               //      }
               // }
               setpgid(0,0);
               if(execvp(process, inputstrings)<0)
                    dprintf(savestdout, "Error in forking\n");

          }

          else if (pid > 0)
          {
               if (!flag){
                    //if fg goes to bg, then add
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
                    //if()
               }
               else{
                    pidStack[pidTop]=(int)pid;
                    strcpy(processStack[pidTop],process);
                    running[pidTop]=1;
                    pidTop++;
                    //add bg processes to stack
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
     //dprintf(savestdout,"%sfirst\n", inputstrings[0]);
     pid2=fork();
     //dprintf(savestdout,"%ssecond\n", inputstrings[0]);
     //vartest=99;
     if (pid2==0) {

          //printf("%s\n", inputstrings[0]);
          //printf("%d\n", words);


          dup2(pd[1], 1); // remap output back to parent
          //return 0;
          //else{
          //close(pd[0]);
          // dprintf(savestdout,"ewiufhjkwef\n");

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
               //int status;
               //pidlol=fork();
               //if(pidlol==0){
               // if(inp>1){
               //      int fd0;
               //      if ((fd0 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
               //           perror("Couldn't open input file");
               //           exit(0);
               //      }
               //      words-=2;
               //      dup2(fd0, 0);
               //      close(fd0);
               // }
               execute(path,flag,savestdout);
               //dup2(savestdin,0);
               abort();
               //abort();
               //}
               //else{

               // int status;
               // waitpid(pidlol, &status, 0);
               // abort();
               // if(status==0){
               //      printf("\n%s with pid %d exited normally\n",process,(int)pidlol);
               // }
               // else{
               //      printf("\n%s with pid %d exited abnormally\n",process,(int)pidlol);
               // }


               // while (!(wait(&status) == pid)) ;
          }
          //return 0;



          execute(path,flag,savestdout);






          abort();
          //}
          //execlp(argv[i], argv[i], NULL);
          //perror("exec");
     }
     else if (pid2<0)
     dprintf(savestdout,"error\n");
     else{
          //dprintf(savestdout,"lolazo\n");
          int status;
          waitpid(pid2,&status,0);
          //dprintf(savestdout,"lolazo2\n");
          dup2(pd[0],0);
          close(pd[1]);
     }
     //printf("lol\n");
     // remap output from previous child to input
     // int status2;
     // waitpid(pid2, &status2, 0);
     //dup2(pd[0], 0);
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
     // pidstack[pidtop]=mainpid;
     // strcpy(processstack[pidtop++],"shell");
     while(exitf){
          signal(SIGCHLD, procExit);
          signal(SIGINT, handler);
          signal(SIGTSTP, handler);
          //printf("lol\n");
          currentadress(path,lenpath,size);
          //printf("lol\n");
          char *inpt;
          // inpt=(char*)malloc(1024);
          //printf("lolazo\n");
          inpt = readline("");
          add_history(inpt);
          //printf("lolazo\n");
          strcpy(processstack[pidtop++],inpt);
          //printf("lolazo\n");
          // for(int i=0; i<pidtop; i++){
          //      printf("%s\n",processstack[i]);
          // }
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
          //printf("lolazo\n");
          //printf("%d\n",pidtop);
          inputsplit(inpt);
          int comnum=0;
          while(numcom){
               signal(SIGCHLD, procExit);
               signal(SIGINT, handler);
               signal(SIGTSTP, handler);
               //printf("%s\n",commands[comnum]);
               //printf("lol1\n");
               int flag=0;
               if(commands[comnum][strlen(commands[comnum])-1] == '&' ){
                    flag=1;
                    commands[comnum][strlen(commands[comnum])-1] = '\0';
               }
               pipesplit(commands[comnum]);
               int numpipe=0, vartest=0, checkpipe=pipes;
               //printf("%s\n",pipestrings[0]);
               // printf("%s\n",pipestrings[1]);
               //printf("%d\n",pipes);
               int countazo=0;
               while(pipes){
                    signal(SIGCHLD, procExit);
                    signal(SIGINT, handler);
                    signal(SIGTSTP, handler);
                    //dprintf(savestdout,"%dpipes\n",pipes);
                    //printf("lol2\n");
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
                    //tempstring3[0]='\0';
                    //strcat(tempstring3,tempstring2);
                    //dprintf(savestdout,"LULLI SACHAN %s\n",tempstring3);

                    //printf("%s1\n",pipestrings[1]);
                    inputredsplit(tempstring);
                    //printf("%s2\n",pipestrings[1]);
                    outputredsplit(tempstring2);
                    //printf("%s3\n",pipestrings[1]);
                    commandsplit(tempstring3);
                    doutputredsplit(tempstring4);
                    //dprintf(savestdout,"LULLI SACHAN %s\n",tempstring3);
                    //dprintf(savestdout,"%d\n",out);
                    //dprintf(savestdout,"%d\n",dout);
                    //int argument_count=commandsplit(tempstring3);
                    // dprintf(savestdout,"LULLI SACHAN %s\n",tempstring3);
                    // inputstrings[argument_count][0]='\0';
                    // strcpy(inputstrings[argument_count][0],"\0");
                    // dprintf(savestdout,"LULLI SACHAN %s\n",tempstring3);
                    //dprintf(savestdout,"%s %d\n",inputstrings[0], vartest);
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
                                   //pipestrings[numpipe][i]='\0';
                                   //strcpy(comm,pipestring[numpipe]);
                              }
                         }
                         if(pipestrings[numpipe][i]=='<'){
                              l=1;
                              inpind=i;
                              //pipestrings[numpipe][i]='\0';
                              //strcpy(comm,pipestring[numpipe]);
                         }
                         if(pipestrings[numpipe][i]=='>' && flagout==0){
                              r=1;
                              outind=i;
                              //pipestrings[numpipe][i]='\0';
                              //strcpy(comm,pipestring[numpipe]);
                         }
                    }
                    //printf("%s4\n",pipestrings[1]);
                    //printf("%s\n",pipestrings[numpipe]);
                    //printf("wtf\n");
                    //printf("%d\n",inp);
                    if(pipes>1){
                         countazo++;
                         execpipe(path, flag,  savestdout, numpipe,l);
                    }
                    else{
                         int flagazo=0;
                         dup2(savestdout, 1);
                         // char comm[200];
                         // char infile[200];
                         // char outfile[200];
                         //printf("%d %d\n",l,r);
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
                              dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                              close(fd1);
                         }

                         if(l==0 && r==2){
                              int fd1 ;
                              if ((fd1 = open(inputstrings[words-1],O_APPEND|O_CREAT|O_WRONLY, 0666)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              words-=2;
                              dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                              close(fd1);
                         }
                         if(l==1 && r==1){
                              int fd0;
                              if ((fd0 = open(inputstrings[words-3], O_RDONLY, 0)) < 0) {
                                   perror("Couldn't open input file");
                                   exit(0);
                              }
                                        //words-=4;
                              dup2(fd0, 0);
                              close(fd0);
                              int fd1 ;
                              if ((fd1 = creat( inputstrings[words-1], 0644)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                              close(fd1);
                              words-=4;
                         }
                         if(l==1 && r==2){
                              int fd0;
                              if ((fd0 = open(inputstrings[words-3], O_RDONLY, 0)) < 0) {
                                   perror("Couldn't open input file");
                                   exit(0);
                              }
                                        //words-=4;
                              dup2(fd0, 0);
                              close(fd0);
                              int fd1 ;
                              if ((fd1 = open(inputstrings[words-1],O_APPEND|O_CREAT|O_WRONLY, 0666)) < 0) {
                                   perror("Couldn't open the output file");
                                   exit(0);
                              }
                              dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                              close(fd1);
                              words-=4;
                         }
                         // if(countazo==0 && countazo==checkpipe-1){
                         //      printf("%d %d\n",inp, out);
                         //      if(inp>1 && out>1){
                         //           flagazo=1;
                         //           int fd0;
                         //           if ((fd0 = open(inputstrings[words-3], O_RDONLY, 0)) < 0) {
                         //                perror("Couldn't open input file");
                         //                exit(0);
                         //           }
                         //           //words-=4;
                         //           dup2(fd0, 0);
                         //           close(fd0);
                         //           int fd1 ;
                         //           if ((fd1 = creat( inputstrings[words-1], 0644)) < 0) {
                         //                perror("Couldn't open the output file");
                         //                exit(0);
                         //           }
                         //           dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                         //           close(fd1);
                         //           words-=4;
                         //      }
                         // }
                         // if(countazo==0 && flagazo==0){
                         //      if(inp>1){
                         //
                         //           //printf
                         //           int fd0;
                         //           if ((fd0 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
                         //                perror("Couldn't open input file");
                         //                exit(0);
                         //           }
                         //           words-=2;
                         //           dup2(fd0, 0);
                         //           close(fd0);
                         //      }
                         // }
                         // if(countazo==checkpipe-1 && flagazo==0){
                         //      if(out>1){
                         //           int fd1 ;
                         //           if ((fd1 = creat( inputstrings[words-1], 0644)) < 0) {
                         //                perror("Couldn't open the output file");
                         //                exit(0);
                         //           }
                         //           words-=2;
                         //           dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
                         //           close(fd1);
                         //      }
                         // }
                         //printf("sdjknf");
                         //printf("Pid in else = %d", pid2);
                         //printf("sdjknf");
                         // if(inp>1){
                         //      printf
                         //      int fd0;
                         //      if ((fd0 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
                         //           perror("Couldn't open input file");
                         //           exit(0);
                         //      }
                         //      words-=2;
                         //      dup2(fd0, 0);
                         //      close(fd0);
                         // }
                         //int fd1;
                         // if(inp==1 && out>1){
                         //      if ((fd1 = open(inputstrings[words-1], O_RDONLY, 0)) < 0) {
                         //           perror("Couldn't open input file");
                         //           exit(0);
                         //      }
                         //      words-=2;
                         //      dup2(fd1,1);
                         //      //close(fd1);
                         // }
                         execute(path,flag, savestdout);
                         //close(fd1);
                         //dup2(savestdout, 1);
                         //printf("wehjkfwef");
                         // execlp(argv[i], argv[i], NULL);
                         // perror("exec");
                         //abort();
                    }
                    //printf("%send\n",pipestrings[1]);
                    pipes--;
                    numpipe++;
                    words=0;
                    inp=0;
                    out=0;
                    //dup2(savestdin, 0);
               }
               dup2(savestdin, 0);
               dup2(savestdout, 1);
               //commandsplit(commands[comnum]);
               //execute(path,flag);
               comnum++;
               numcom--;
               //printf("lol3\n");
               //words=0;
          }
          free(inpt);
     }
     return 0;
}
