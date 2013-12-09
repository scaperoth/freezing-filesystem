#include <string.h>
#include <stdio.h>

#define BUFF 1000

int main(int argc, char *argv[])
{
    char command[BUFF] = "";
    if (strcmp(argv[1], "log") == 0)
    {
        if (strcmp(argv[2], "") != 0)
        {
            strncat(command, "echo \"", 10);
            strncat(command, argv[2], 400);
            strncat(command, "\" >> /root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_linkedlist/log.txt", 150);
            system(command);
        }
    }

    if (strcmp(argv[1], "snapshot") == 0)
    {
        
    }

    if (strcmp(argv[1], "restore") == 0)
    {
        
    }



    return 0;
}
