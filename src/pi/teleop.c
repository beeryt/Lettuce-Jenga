#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h> // getopt
#include <stdlib.h>
#include <stdarg.h>

FILE* file_open(const char* fn, const char* m);
void usb_out(FILE*, const char*, ...);

const int TOWER_HEIGHT = 800;

const int block_heights[18] = 
{
	888 + 73 * 0,
	888 + 73 * 1,
	888 + 73 * 2,
	888 + 73 * 3,
	888 + 73 * 4,
	888 + 73 * 5,
	888 + 73 * 6,
	888 + 73 * 7,
	888 + 73 * 8,
	888 + 73 * 9,
	888 + 73 * 10,
	888 + 73 * 11,
	888 + 73 * 12,
	888 + 73 * 13,
	888 + 73 * 14,
	888 + 73 * 15,
	888 + 73 * 16,
	888 + 73 * 17
};

const int max_z = 1713;
int delta_x = 0;
int delta_y = 0;
int delta_z = 0;

void open_gripper(FILE* fp);
void close_gripper(FILE* fp);
void move_level(FILE* fp);
void move_rotate(FILE* fp);
void move_forward(FILE* fp);
void move_backward(FILE* fp);
void home(FILE* fp, char axis);
void home_prompt(FILE* fp);

void status()
{
	printf("delta_x: %d\n", delta_x);
	printf("delta_y: %d\n", delta_x);
	printf("delta_z: %d\n", delta_x);
}

void help()
{
	printf("Supported Commands:\n");
	printf("Gripper:\n");
	printf("\tOpen -- Opens the gripper (assumes gripper is moved back)\n");
	printf("\tClose -- Moves gripper forward, grips, and retracts gripper\n");
	printf("X-axis\n");
	printf("\tforward -- Moves robot forward over tower\n");
	printf("\tback -- Moves robot back to player area\n");
	printf("Y-axis:\n");
	printf("\trotate -- choose a rotation position\n");
	printf("Z-axis:\n");
	printf("\tlevel -- move to a specific block level\n");
}

int main(int argc, char** argv)
{
	FILE* file_stepper;
	FILE* file_gripper;

	int opt, dryrun = 0;
	while ((opt = getopt(argc, argv, "d")) != -1)
	{
		if (opt == 'd') dryrun = 1;
		else
		{
			fprintf(stderr, "Usage: %s [-d] [file1 file2]\n", argv[0]);
			exit(1);
		}
	}

	if (dryrun)
	{
		file_stepper = file_open("/dev/null", "r+");
		file_gripper = file_open("/dev/null", "r+");
	}
	else if (argc - optind == 1) // specified filenames
	{
		file_stepper = file_open(argv[1], "r+");
		file_gripper = file_open(argv[2], "r+");
	}
	else if (!dryrun) // default filenames
	{
		file_stepper = file_open("/dev/ttyACM1", "r+");
		file_gripper = file_open("/dev/ttyACM0", "r+");
	}

	while (1)
	{
		char* input = readline("> ");
		add_history(input);

		if (!strcmp(input, "exit")) break;
		else if (!strcmp(input, "open")) open_gripper(file_gripper);
		else if (!strcmp(input, "close")) close_gripper(file_gripper);
		else if (!strcmp(input, "level")) move_level(file_stepper);
		else if (!strcmp(input, "rotate")) move_rotate(file_stepper);
		else if (!strcmp(input, "back")) move_backward(file_stepper);
		else if (!strcmp(input, "forward")) move_forward(file_stepper);
		else if (!strcmp(input, "status")) status();
		else if (!strcmp(input, "help")) help();
		else if (!strcmp(input, "home")) home_prompt(file_stepper);
		else if (!strcmp(input, "hz")) home(file_stepper, 'z');
		// else if (!strcmp(input, "hy")) home(file_stepper, 'y');

		else switch (input[0])
		{
			case 'x':
			case 'y':
			case 'z':
			case 'h': usb_out(file_stepper, "%s", input);
			break;

			case 'g':
			case 'u':
			case 'f':
			case 'b': usb_out(file_gripper, "%s", input);
			break;

			default: fprintf(stderr, "teleop command not supported: %s\n", input);
		}
		free(input);
	}

	printf("Exiting\n");
	fclose(file_stepper);
	fclose(file_gripper);
}

/**
 * @brief      Opens a file and exits on fail
 */
FILE* file_open(const char* fn, const char* m)
{
	FILE* ret = fopen(fn, m);
	if (ret == NULL)
	{
		perror(fn);
		exit(1);
	}
	return ret;
}

/**
 * @brief      Writes to a file and flushes the output
 */
void usb_out(FILE* fp, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	vfprintf(fp, format, args); // write to file
	fflush(fp);					// flush output

	va_end(args);
}

void open_gripper(FILE* fp)
{
	usb_out(fp, "u");
}

void close_gripper(FILE* fp)
{

	usb_out(fp, "2-300");
	usb_out(fp, "u");
	usb_out(fp, "2-700");

	// usb_out(fp, "f");
	char* input = readline("Are you sure? ");
	if (!strcmp(input, "y")) // not a match
	{
		usb_out(fp, "g");
		usleep(50000);
		usb_out(fp, "b");
	}
	else
	{
		usb_out(fp, "b");
	}
}

void move_level(FILE* fp)
{
	char* input = readline("Which block (1-18)? ");
	for (uint i = 0; i < strlen(input); ++i)
		if (!isdigit(input[i])) return;

	int lvl = atoi(input) - 1;
	int steps = block_heights[lvl] - delta_z;
	printf("Moving z-axis %d steps\n", steps);

	if (steps + delta_z > max_z)
	{
		printf("Move would exceed max z-axis position!!!\n");
		return;
	}

	usb_out(fp, "z%d", steps);
	delta_z += steps;
}

void move_forward(FILE* fp)
{
	if (delta_z > TOWER_HEIGHT) home(fp, 'z');
	int steps = 1450;
	printf("Moving x-axis %d steps\n", steps);
	usb_out(fp, "y%d", steps);
	delta_x += steps;
}

void home(FILE* fp, char axis)
{
	if (axis == 'z')
	{
		if (delta_y) home(fp, 'y');
		printf("Homing z-axis\n");
		delta_z = 0;
		usb_out(fp, "hz");
		usleep(5000000);
	}
	else if (axis == 'y')
	{
		printf("Homing y-axis\n");
		delta_y = 0;
		usb_out(fp, "hy");
		usleep(8000000);
		usb_out(fp, "y-463");
		delta_y += -463;
	}
}

void home_prompt(FILE* fp)
{
	char* input = readline("Which Axis (z/y)? ");
	if (!strcmp(input, "y")) home(fp, 'y');
	if (!strcmp(input, "z")) home(fp, 'z');
}

void move_backward(FILE* fp)
{
	if (delta_z) home(fp, 'z');
	int steps = -1450;
	printf("Moving x-axis %d steps\n", steps);
	usb_out(fp, "y%d", steps);
	delta_x += steps;
}

void move_rotate(FILE* fp)
{
	char* input = readline("Which position would you like (left/center/right)? ");

	int steps = 0;
	if (!strcmp(input, "left"))		steps = -463 + 463 - delta_y;
	if (!strcmp(input, "center"))	steps = -463 - delta_y;
	if (!strcmp(input, "right"))	steps = -463 - 463 - delta_y;

	printf("Moving y-axis %d steps\n", steps);
	usb_out(fp, "y%d", steps);
	delta_y += steps;
}