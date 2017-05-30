#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#include "../salad_spinner_global.h"

void forward();
void back();
void up();
void down();

FILE* file_stepper;
FILE* file_gripper;

#define STATUS_X_DELTA(x) (int)x[80];

int main(void)
{

	// Get filenames from user
	char* file_teensy1 = readline("Enter the location of the stepper teensy: ");
	file_stepper = fopen(file_teensy1, "r+");
	if (file_stepper == NULL)
	{
		perror("file_teensy1");
		exit(1);
	}

	char* file_teesny2 = readline("Enter the location of the gripper teensy: ");
	file_gripper = fopen(file_teesny2, "r+");
	if (file_gripper == NULL)
	{
		perror("file_teensy2");
		exit(1);
	}

	int i = 0;

	// Shell loop
	while (1)
	{
		// Prompt user
		char* input = readline("> ");
		if (input == 0) break;
		add_history(input);

		if (!strcmp(input, "forward")) forward();
		else if (!strcmp(input, "back")) back();
		else if (!strcmp(input, "up")) up();
		else if (!strcmp(input, "down")) down();
		else if (!strcmp(input, "exit")) break;

		// catch general commands
		else switch (input[0])
		{
			case 'x':
			case 'y':
			case 'z':
			case 'h':	i = fprintf(file_stepper, "%s", input);
						fflush(file_stepper);
						if (i < 0) perror("fprintf: file_stepper");
			break;

			case 'g':
			case 'u':
			case 'f':
			case 'b':	i = fprintf(file_gripper, "%s", input);
						fflush(file_gripper);
						if (i < 0) perror("fprintf: file_gripper");
			break;

			default: printf("teleop command not supported: %d\n", input[0]);
		}
		free(input);
	}

	printf("\nExiting\n");
	fclose(file_stepper);
	fclose(file_gripper);
}

void forward()
{
	int steps = STEPS_TO_DEPLOY;
	printf("Will take %d steps\n", steps);
}

void back()
{
	char status[50];
	fprintf(file_stepper, "s");
	fscanf(file_stepper, "%s", status);

	int delta = STATUS_X_DELTA(status);
	printf("Will take %d\n", -delta);
}

void up()
{

}

void down()
{
	char* input = readline("Which layer (from the top)? ");
	if (input == 0) return;
	for (uint i = 0; i < strlen(input); ++i)
		if (!isdigit(input[i])) return;

	int steps = STEPS_TO_TOWER_TOP + STEPS_PER_BLOCK * atoi(input);
	printf("Will take %d steps.\n", steps);

}
