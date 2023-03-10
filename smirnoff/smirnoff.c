#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"
#define MAX_LINE_LENGTH 64
#define COLUMNS 16
#define ROWS 28
#define MAX_BUFFER_SIZE COLUMNS * ROWS
#define UNPRINTABLE_PLACEHOLDER "."
#define KEY_SUFFIX "/CK"



void print_app_info(char *color);
void print_usage();
void read_memdump(char *filename, unsigned char *buffer);
int find_bytes(unsigned char *haystack, unsigned char *needle, int hc);
void print_grid_header();
void print_row(int row_number, unsigned char *bytes, int buffer_index, int key_index, int key_length);
void highlight_key(int buffer_index, int row_index, int key_index, int key_length, char *color);
void set_color(char *color);



int main(int argc, char *argv[])
{
	print_app_info(GREEN);

	if (argc != 2)
	{
		print_usage(argv[0]);
		return 0;
	}

	unsigned char buffer[MAX_BUFFER_SIZE];
	unsigned char row[COLUMNS];

	read_memdump(argv[1], buffer);

	printf("\nAnalyzing memory dump...\n\n");

	// Find key suffix
	int ck_index = find_bytes(buffer, KEY_SUFFIX, MAX_BUFFER_SIZE);

	// Work backwards from key suffix until the first non alpha-numeric character is found
	int key_index = ck_index - 1;
	while (isalnum(buffer[--key_index - 1]))
		;

	// Display results
	int current_row = 0;

	print_grid_header();

	for (int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		row[i % COLUMNS] = buffer[i];

		if ((i > 0 && i % COLUMNS == 0) || (i == MAX_BUFFER_SIZE - 1))
		{
			print_row(current_row++, row, i, key_index, ck_index - key_index);
		}
	}

	printf("\n");
	printf("Key Offset (Hex): %03x\n\n", key_index);
}



void print_app_info(char * color)
{
	printf("smirnoff 1.0 ("
		"%sS" RESET "atan "
		"%sM" RESET "emdump "
		"%sI" RESET "nspecto%sR" RESET " "
		"a%sN" RESET "d "
		"%sOF" RESET "fset "
		"%sF" RESET "inder)\n"
		,color, color, color, color, color, color, color, color);
}



void print_usage()
{
	printf("Usage: smirnoff <filename>\n");
}



void read_memdump(char *filename, unsigned char *buffer)
{
	FILE *fp;
	char line[MAX_LINE_LENGTH];
	unsigned char b;
	int buffer_length = 0;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("File not found '%s'\n", filename);
		exit(1);
	}

	// Discard the first 2 lines
	fgets(line, MAX_LINE_LENGTH, fp);
	fgets(line, MAX_LINE_LENGTH, fp);

	for (int r = 0; r < ROWS; r++)
	{

		// Discard the first 5 bytes
		for (int i = 0; i < 5; i++)
			getc(fp);

		// Read 16 bytes
		for (int i = 0; i < COLUMNS; i++)
		{
			fscanf(fp, "%2x", &buffer[buffer_length++]);

			// Ignore whitespace (space and newline chars)
			b = getc(fp);
		}
	}

	fclose(fp);
}



int find_bytes(unsigned char *haystack, unsigned char *needle, int hc)
{
	int haystack_index = 0;
	int needle_index = 0;
	int nc = strlen(needle);

	while (haystack_index < hc)
	{
		if (haystack[haystack_index] == needle[needle_index])
		{
			if (needle_index == nc - 1)
				return (haystack_index - nc) + 1;

			++needle_index;
		}
		else
			needle_index = 0;

		++haystack_index;
	}

	return -1;
}



void print_grid_header()
{
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("     -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -\n");
}



void print_row(int row_number, unsigned char *bytes, int buffer_index, int key_index, int key_length)
{
	int index;

	printf("%02x | ", row_number);

	for (int i = 0; i < COLUMNS; i++)
	{
		highlight_key(buffer_index, i, key_index, key_length, GREEN);

		printf("%02x", bytes[i]);
		if (i < COLUMNS)
			printf(" ");
	}

	printf("\t");

	for (int i = 0; i < COLUMNS; i++)
	{
		highlight_key(buffer_index, i, key_index, key_length, GREEN);

		if (isprint(bytes[i]))
			printf("%c", bytes[i]);
		else
			printf(UNPRINTABLE_PLACEHOLDER);
	}

	printf("\n");
}



void highlight_key(int buffer_index, int row_index, int key_index, int key_length, char *color)
{
	int index = buffer_index - COLUMNS + row_index;

	if ((index >= key_index) && (index < (key_index + key_length)))
		set_color(color);
	else
		set_color(RESET);
}



void set_color(char *color)
{
	printf(color);
}