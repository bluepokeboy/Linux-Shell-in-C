#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "address.h"
void currentadress(char *path, int lenpath, int size)
{
     char cwd[size];
     getcwd(cwd, size);
     if (strncmp(cwd, path, lenpath) == 0)
     {
          fprintf(stderr, "<%s@%s:~%.*s>", username, systemname, size, cwd + lenpath);
     }
     else
     {
          fprintf(stderr, "<%s@%s:%s>", username, systemname, cwd);
     }
}

char *namefrompath(char *path)
{
     for (int i = strlen(path) - 1; i; i--)
     {
          if (path[i] == '/')
          {
               return &path[i + 1];
          }
     }
     return path;
}
