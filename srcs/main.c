#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>

static void read_file(const char *filename, char **content)
{
    if (access(filename, F_OK) != 0)
    {
        fprintf(stderr, "ft_ssl: %s: No such file or directory\n", filename);
        return;
    }

    if (access(filename, R_OK) != 0)
    {
        fprintf(stderr, "ft_ssl: %s: Permission denied\n", filename);
        return;
    }

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "ft_ssl: %s: %s\n", filename, strerror(errno));
        /* NEVER HERE */
        ft_assert(file, "Fatal error: Could not open file.");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *content = malloc(file_size + 1);

    size_t read_size = fread(*content, 1, file_size, file);
    if (read_size != (size_t)file_size)
    {
        perror("Error reading file");
        free(*content);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    (*content)[file_size] = '\0';

    fclose(file);
}

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return INVALID_ARGS;
    }

    char *content = NULL;
    read_file(argv[1], &content);

    printf("%s\n", content);

    free(content);

    return OK;
}