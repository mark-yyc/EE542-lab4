#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define FILE_SIZE (1L * 1024 * 1024 * 1024) // 1GB
#define FILE_SIZE (1L * 1024 * 1024 * 1024)
#define FILE_NAME "file.txt"
#define BUFFER_SIZE 1024 // Buffer size for writing to the file

// Function to generate a random alphabetic character (A-Z, a-z)
char get_random_letter()
{
    int is_upper = rand() % 2; // Randomly choose uppercase or lowercase
    if (is_upper)
    {
        return 'A' + (rand() % 26); // Generate a random uppercase letter
    }
    else
    {
        return 'a' + (rand() % 26); // Generate a random lowercase letter
    }
}

int main()
{
    FILE *file;
    char buffer[BUFFER_SIZE];
    long total_written = 0;

    // Seed the random number generator
    srand(time(NULL));

    // Open the file for writing
    file = fopen(FILE_NAME, "wb");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Generate and write random letters until we reach 1GB
    while (total_written < FILE_SIZE)
    {
        // Fill the buffer with random letters
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            buffer[i] = get_random_letter();
        }

        // Write the buffer to the file
        size_t written = fwrite(buffer, 1, BUFFER_SIZE, file);
        if (written != BUFFER_SIZE)
        {
            perror("Error writing to file");
            fclose(file);
            return 1;
        }

        total_written += written;
    }

    fclose(file);

    return 0;
}