/*
 * @author Matt Scaperoth
 * This is the user space helper for the freezing filesystem
 * this file either logs, snapshots, or restores the frozen file
 * based on the 2nd argument (argv[1]). 
 *
 * TODO: change argument from string to constant values or enum
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define _GNU_SOURCE
#define BUFF 2048
#define DEBUG = 0;  /*set to 1 to turn on printf statements*/

const char *FREEZERPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/freezer/";
const char *LOGPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/log.txt";
const char *PROJROOT = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/";
const char *SNAPSHOTPATH = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/snapshot/";

int main(int argc, char *argv[])
{
    char command[BUFF] = "";
    int decider = -1;

    if (argv[1] == NULL){
        if(DEBUG)
            printf("no arguments given\n");
    }

    /*
     * This section changes strings to enumeration for simplicity
     * TODO: create better way of sending these arguments
     */
    if (strcmp(argv[1], "log") == 0)
        decider = 0;

    if (strcmp(argv[1], "snapshot") == 0)
        decider = 1;

    if (strcmp(argv[1], "restore") == 0)
        decider = 2;

    if (decider < 0)
    {
        if(DEBUG)
            printf("not a valid command\n");
        return 0;
    }

    switch (decider)
    {

    /*LOG case*/
    case 1:
        if (strcmp(argv[2], "") == 0)
            return 0;

        strncat(command, "echo \"", 8);
        strncat(command, argv[2], 512);
        strncat(command, "\" >> ", 32);
        strncat(command, LOGPATH, 512);

        system(command);

        break;

    /*SNAPSHOT case*/
    case 2:
        strncat(command, "cp ", 4);
        strncat(command, FREEZERPATH, 512);
        strncat(command, "* ", 4);
        strncat(command, SNAPSHOTPATH, 512);

        system(command);

        break;

    /*
     * RESTORE case
     * code skeleton from getline() man page
     */
    case 3:

        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(LOGPATH, "r");

        if (fp == NULL)
        {
            return 0;
        }

        /*forms command like: "cp /../linefromfile /../freezerpath/; cp...; cp...;"*/
        while ((read = getline(&line, &len, fp)) != -1)
        {
            line[strlen(line) - 1] = 0;

            strncat(command, "cp ", 16);
            strncat(command, line, 512);
            strncat(command, " ", 4);
            strncat(command, FREEZERPATH, 512);
            strncat(command, "; ", 1);

        }

        if(DEBUG)
            printf("Final command: %s", command);

        FILE *fd = fopen(LOGPATH , "w");
        fclose(fd);

        free(line);

        system(command);

        break;
    }

    return 0;
}
