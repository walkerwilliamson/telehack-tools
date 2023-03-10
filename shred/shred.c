#include <crypt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 50

// To compile: gcc shred.c -lcrypt -o shred
// Sample hash: "OHM.7d1emYlF2"
// Usage: shred <hash> <wordlist>
// TODO: hZzB.n5R9wjZs77GW5FdYgxo - crypt16

void print_app_info();
void print_usage();

int main(int argc, char *argv[])
{
    print_app_info();

    if (argc != 3)
    {
        print_usage();
        return 0;
    }
    
    FILE *fp;
    char *hash = argv[1];
    char *wordlist_filename = argv[2];
    char salt[3];
    char word[MAX_WORD_LENGTH];
    bool found = false;

    memcpy(salt, hash, 2);
    salt[2] = 0;

    if ((fp = fopen(wordlist_filename, "r")) == NULL)
    {
        printf("File not found '%s'\n", wordlist_filename);
        exit(1);
    }

    while (fgets(word, sizeof(word), fp))
    {
        word[strcspn(word, "\n")] = 0;

        if (strcmp(crypt(word, salt), hash) == 0)
        {
            found = true;
            break;
        }
    }

    fclose(fp);

    if (found)
        printf("%s:%s\n", hash, word);
    else
        printf("Unable to crack the hash.\n");
}



void print_app_info()
{
    printf("shred 1.0\n");
}



void print_usage()
{
    printf("Usage: shred <hash> <wordlist>\n");
}