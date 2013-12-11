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
#include <syslog.h>

#define _GNU_SOURCE

#define BUFFER_SIZE 2048
#define DEBUG 0 /*set to 1 to turn on printf statements*/
//#define KERNEL_DEBUG

char command[BUFFER_SIZE];

const char *PROJROOT = "/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/";
const char *FREEZERPATH = "freezer/";
const char *LOGPATH = "log.txt";
const char *SNAPSHOTPATH = "snapshot/";

int main(int argc, char *argv[])
{

    char full_log_path[BUFFER_SIZE];
    int decider = -1;
    FILE *fp, *erase_fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argv[1] == NULL)
    {
        if (DEBUG)
            printf("no arguments given\n");
        return -1;
    }

    /*
     * This section changes strings to enumeration for simplicity
     * TODO: create better way of sending these arguments
     */
    if (strcmp(argv[1], "log") == 0)
        decider = 0;

    else if (strcmp(argv[1], "snapshot") == 0)
        decider = 1;

    else if (strcmp(argv[1], "restore") == 0)
        decider = 2;

    else
    {
        if (DEBUG)
            printf("not a valid command\n");

        return -1;
    }

    if (DEBUG)
        printf("Decider: %d\n", decider);

    switch (decider)
    {
    /*LOG case*/
    case 0:
        if (DEBUG)
            printf("in the log case\n");

        if (strcmp(argv[2], "") == 0)
            return 0;

        sprintf(command, "echo \"%s\" >> %s%s", argv[2], PROJROOT, LOGPATH);

        if (DEBUG)
            printf("%s\n", command);

        system(command);

        break;
    /*SNAPSHOT case*/
    case 1:
        if (DEBUG)
            printf("in the SNAPSHOT case\n");

        sprintf(command, "cp %s%s* %s%s", PROJROOT, FREEZERPATH, PROJROOT, SNAPSHOTPATH);

        if (DEBUG)
            printf("%s\n", command);

        system(command);

        break;
    /*RESTORE case*/
    case 2:
        if (DEBUG)
            printf("in the RESTORE case\n");

        sprintf(full_log_path, "%s%s", PROJROOT,  LOGPATH);

        fp = fopen(full_log_path, "r");

        if (fp == NULL)
        {
            if (DEBUG)
                printf("Failed to open file.\n");
            return 0;
        }

        /*forms command like: "cp /../linefromfile /../freezerpath/; cp...; cp...;"*/
        while ((read = getline(&line, &len, fp)) != -1)
        {
            line[strlen(line) - 1] = 0;

            strncat(command, "cp ", 16);
            strncat(command, line, 512);
            strncat(command, " ", 4);
            strncat(command, PROJROOT, 512);
            strncat(command, FREEZERPATH, 32);
            strncat(command, "; ", 1);

        }

        if (DEBUG)
            printf("%s\n", command);

        //erases the contents of the log file
        erase_fp = fopen(full_log_path , "w");
        fclose(erase_fp);

        free(line);

        system(command);

        break;
    }

    return 0;
}
