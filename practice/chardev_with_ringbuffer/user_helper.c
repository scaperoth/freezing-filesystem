#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	char command[10] = "pwd";
	if(argv[1] != NULL) {
		strncat(command, argv[1], 1);
	}
	else
		strncat(command, "1", 1);
	
	system(command);

	FILE *f = fopen("file.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		return 0;
	}

/* print some text */
	const char *text = "Write this to the file";
	fprintf(f, "Some text: %s\n", text);

/* print integers and floats */
	int i = 1;
	float py = 3.1415927;
	fprintf(f, "Integer: %d, float: %f\n", i, py);

/* printing single chatacters */
	char c = 'A';
	fprintf(f, "A character: %c\n", c);

	fclose(f);


	return 0;
}
