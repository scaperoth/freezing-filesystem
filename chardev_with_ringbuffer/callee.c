#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	/*
	char command[30] = "cat log.txt"	;

	printf("callee: %s\n","TEST");
	
	system(command);
	*/
	printf("Opening file...\n");
	FILE *f = fopen("/root/Desktop/scaperoth_csci3411/freezing_filesystem/chardev_with_ringbuffer/log.txt", "w");
	if (f == NULL)
	{
		printf("\nError opening file!\n");
		return 0;
	}	

	printf("Writing to file...\n");

/* print integers and floats */
	int i = 1;
	float py = 3.1415927;
	fprintf(f, "Integer: %d, float: %f\n", i, py);

/* printing single chatacters */
	char c = 'A';
	fprintf(f, "A character: %c\n", c);

	printf("Closing file.\n");
	fclose(f);


	return 0;
}
