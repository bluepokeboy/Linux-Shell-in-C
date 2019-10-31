#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
void inputsplit(char *input)
{
     //printf("lol1\n");
     char *token = strtok(input, ";");
     //printf("%s\n",input);
     while (token != NULL)
     {
          //strcpy(commands[numcom],token);
          commands[numcom] = token;
          //printf("lol2\n");
          numcom++;
          token = strtok(NULL, ";");
     }
}

void pipesplit(char *input)
{
     char *token = strtok(input, "|");
     while (token != NULL)
     {
          pipestrings[pipes] = token;
          pipes++;
          token = strtok(NULL, "|");
     }
}

void inputredsplit(char *input)
{
     char *token = strtok(input, "<");
     while (token != NULL)
     {
          inputredstrings[inp] = token;
          inp++;
          token = strtok(NULL, "<");
     }
}

void outputredsplit(char *input)
{
     char *token = strtok(input, ">");
     while (token != NULL)
     {
          outputredstrings[out] = token;
          out++;
          token = strtok(NULL, ">");
     }
}

void doutputredsplit(char *input)
{
     char *token = strtok(input, ">");
     while (token != NULL)
     {
          doutputredstrings[dout] = token;
          dout++;
          token = strtok(NULL, ">");
     }
}

int commandsplit(char *input)
{
     char *token = strtok(input, " ");
     int ret = 0;
     while (token != NULL)
     {
          inputstrings[words] = token;
          words++;
          token = strtok(NULL, " ");
          ret++;
     }
     return ret;
}
