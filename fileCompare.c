#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main()
{
    FILE *file1, *file2;
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    int line_number = 1;

    // Open the first file
    file1 = fopen("file.txt", "r");
    if (file1 == NULL)
    {
        perror("Error opening file.txt");
        return 1;
    }

    // Open the second file
    file2 = fopen("output.txt", "r");
    if (file2 == NULL)
    {
        perror("Error opening output.txt");
        fclose(file1);
        return 1;
    }

    // Read and compare the files line by line
    while (fgets(buffer1, BUFFER_SIZE, file1) != NULL && fgets(buffer2, BUFFER_SIZE, file2) != NULL)
    {
        // Compare the two lines
        if (strcmp(buffer1, buffer2) != 0)
        {
            printf("Difference found at line %d:\n", line_number);
            printf("file.txt:    %s\n", buffer1);
            printf("output.txt:  %s\n", buffer2);
        }
        line_number++;
    }

    // Handle if one file has extra lines
    if (fgets(buffer1, BUFFER_SIZE, file1) != NULL)
    {
        printf("file.txt has extra content at line %d: %s\n", line_number, buffer1);
    }
    if (fgets(buffer2, BUFFER_SIZE, file2) != NULL)
    {
        printf("output.txt has extra content at line %d: %s\n", line_number, buffer2);
    }

    // Close the files
    fclose(file1);
    fclose(file2);

    return 0;
}
