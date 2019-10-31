#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include<sys/wait.h>
#include"address.h"
extern int size;
extern int words;
extern char *inputstrings[200];
void ls(char *path){
     DIR *d;
     struct dirent *dir;
     struct stat fileStat;
     d = opendir(".");
     if(words==1){
          if(d!=NULL){
               while ((dir = readdir(d)) != NULL)
                    {
                         if(strncmp(dir->d_name,".",size)!=0 && strncmp(dir->d_name,"..",size)!=0){
                              printf("%s\n", dir->d_name);
                         }
                    }
               closedir(d);
          }
          else{
               printf("Could not open current directory\n");
          }
     }
     else if(words==2){
          if(strncmp(inputstrings[1],"-a",size)==0){
               if(d!=NULL){
                    while ((dir = readdir(d)) != NULL)
                         {
                              printf("%s\n", dir->d_name);
                         }
                    closedir(d);
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
          else if(strncmp(inputstrings[1],"-l",size)==0){
               if(d!=NULL){
                    char fullpath[200];
                    while ((dir = readdir(d)) != NULL)
                         {
                              char mod[20];
                              strcpy(fullpath,path);
                              strcat(fullpath,"/");
                              strcat(fullpath,dir->d_name);
                              if(strncmp(dir->d_name,".",size)!=0 && strncmp(dir->d_name,"..",size)!=0){
                                   if (!stat(fullpath, &fileStat)){
                                        printf((S_ISDIR(fileStat.st_mode))  ? "d" : "-");
                                        printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                                        printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                                        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                                        printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                                        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                                        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                                        printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                                        printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                                        printf((fileStat.st_mode & S_IXOTH) ? "x " : "- ");
                                        printf("%d ",fileStat.st_nlink);
                                        struct passwd *pw = getpwuid(fileStat.st_uid);
                                        struct group  *gr = getgrgid(fileStat.st_gid);
                                        printf("%s %s ", pw->pw_name, gr->gr_name);
                                        printf("%lld ",fileStat.st_size);
                                        strftime(mod, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
                                        printf("%s",mod);
                                        printf("%s\n", dir->d_name);
                                   }
                              }
                         }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
          else if(strncmp(inputstrings[1],"-la",size)==0 || strncmp(inputstrings[1],"-al",size)==0){
               if(d!=NULL){
                    char fullpath[200];
                    while ((dir = readdir(d)) != NULL)
                         {
                              char mod[20];
                              strcpy(fullpath,path);
                              strcat(fullpath,"/");
                              strcat(fullpath,dir->d_name);

                              if (!stat(fullpath, &fileStat)){
                                   printf((S_ISDIR(fileStat.st_mode))  ? "d" : "-");
                                   printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                                   printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                                   printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                                   printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                                   printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                                   printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                                   printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                                   printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                                   printf((fileStat.st_mode & S_IXOTH) ? "x " : "- ");
                                   printf("%d ",fileStat.st_nlink);
                                   struct passwd *pw = getpwuid(fileStat.st_uid);
                                   struct group  *gr = getgrgid(fileStat.st_gid);
                                   printf("%s %s ", pw->pw_name, gr->gr_name);
                                   printf("%lld ",fileStat.st_size);
                                   strftime(mod, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
                                   printf("%s",mod);
                                   printf("%s\n", dir->d_name);
                              }
                         }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
          else{
               if(!stat(inputstrings[1],&fileStat)){
                    if(S_ISREG(fileStat.st_mode)){
                         printf("%s\n",namefrompath(inputstrings[1]));
                    }
                    if(S_ISDIR(fileStat.st_mode)){
                         DIR *d2;
                         struct dirent *dir2;
                         d2 = opendir(inputstrings[1]);
                         if(d2!=NULL){
                              while ((dir2 = readdir(d2)) != NULL)
                                   {
                                        if(strncmp(dir2->d_name,".",size)!=0 && strncmp(dir2->d_name,"..",size)!=0){
                                             printf("%s\n", dir2->d_name);
                                        }
                                   }
                              closedir(d2);
                         }
                    }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
     }
     else if(words==3){
          if(strncmp(inputstrings[1],"-a",size)==0){
               if(!stat(inputstrings[2],&fileStat)){
                    if(S_ISREG(fileStat.st_mode)){
                         printf("%s\n",namefrompath(inputstrings[2]));
                    }
                    if(S_ISDIR(fileStat.st_mode)){
                         DIR *d2;
                         struct dirent *dir2;
                         d2 = opendir(inputstrings[2]);
                         if(d2!=NULL){
                              while ((dir2 = readdir(d2)) != NULL)
                                   {
                                             printf("%s\n", dir2->d_name);
                                   }
                              closedir(d2);
                         }
                    }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
          else if(strncmp(inputstrings[1],"-l",size)==0){
               if(!stat(inputstrings[2],&fileStat)){
                    if(S_ISREG(fileStat.st_mode)){
                         printf("%s\n",namefrompath(inputstrings[2]));
                    }
                    if(S_ISDIR(fileStat.st_mode)){
                         DIR *d2;
                         struct dirent *dir2;
                         d2 = opendir(inputstrings[2]);
                         if(d2!=NULL){
                              char fullpath[200];
                              while ((dir2 = readdir(d2)) != NULL)
                                   {
                                        char mod[20];
                                        strcpy(fullpath,inputstrings[2]);
                                        strcat(fullpath,"/");
                                        strcat(fullpath,dir2->d_name);
                                        if(strncmp(dir2->d_name,".",size)!=0 && strncmp(dir2->d_name,"..",size)!=0){
                                             if (!stat(fullpath, &fileStat)){
                                                  printf((S_ISDIR(fileStat.st_mode))  ? "d" : "-");
                                                  printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                                                  printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                                                  printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXOTH) ? "x " : "- ");
                                                  printf("%d ",fileStat.st_nlink);
                                                  struct passwd *pw = getpwuid(fileStat.st_uid);
                                                  struct group  *gr = getgrgid(fileStat.st_gid);
                                                  printf("%s %s ", pw->pw_name, gr->gr_name);
                                                  printf("%lld ",fileStat.st_size);
                                                  strftime(mod, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
                                                  printf("%s",mod);
                                                  printf("%s\n", dir2->d_name);
                                             }
                                        }
                                   }

                              closedir(d2);
                         }
                    }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
          else if(strncmp(inputstrings[1],"-la",size)==0 || strncmp(inputstrings[1],"-al",size)==0){
               if(!stat(inputstrings[2],&fileStat)){
                    if(S_ISREG(fileStat.st_mode)){
                         printf("%s\n",namefrompath(inputstrings[2]));
                    }
                    if(S_ISDIR(fileStat.st_mode)){
                         DIR *d2;
                         struct dirent *dir2;
                         d2 = opendir(inputstrings[2]);
                         if(d2!=NULL){
                              char fullpath[200];
                              while ((dir2 = readdir(d2)) != NULL)
                                   {
                                        char mod[20];
                                        strcpy(fullpath,inputstrings[2]);
                                        strcat(fullpath,"/");
                                        strcat(fullpath,dir2->d_name);
                                             if (!stat(fullpath, &fileStat)){
                                                  printf((S_ISDIR(fileStat.st_mode))  ? "d" : "-");
                                                  printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                                                  printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                                                  printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                                                  printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                                                  printf((fileStat.st_mode & S_IXOTH) ? "x " : "- ");
                                                  printf("%d ",fileStat.st_nlink);
                                                  struct passwd *pw = getpwuid(fileStat.st_uid);
                                                  struct group  *gr = getgrgid(fileStat.st_gid);
                                                  printf("%s %s ", pw->pw_name, gr->gr_name);
                                                  printf("%lld ",fileStat.st_size);
                                                  strftime(mod, 20, "%b %d %H:%M ", localtime(&(fileStat.st_mtime)));
                                                  printf("%s",mod);
                                                  printf("%s\n", dir2->d_name);
                                             }
                                   }

                              closedir(d2);
                         }
                    }
               }
               else{
                    printf("Could not open current directory\n");
               }
          }
     }
     else{
          printf("Incorrect number of arguments have been given.\n");
     }
}
