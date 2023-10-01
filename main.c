#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>
#include <sys/stat.h> // Needed for checking file existence

void process_line(char *line, GHashTable *df_data, char **header)
{
}

void get_headers(char *line, GHashTable *df_data, GPtrArray *header_array)
{
    size_t len = strlen(line);
    char *current_header = line;

    for (size_t i = 0; i < len; ++i)
    {
        if (line[i] == ',' || line[i] == '\0')
        {
            line[i] = '\0';
            g_ptr_array_add(header_array, strdup(current_header));
            current_header = line + i + 1; // Start of the next cell
        }
    }
}

bool file_exists(const char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool check_csv_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return false;
    return strcmp(dot, ".csv") == 0;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Please provide exactly one argument: the path to the CSV file\n");
        return 1;
    }

    char *filename = argv[1];

    if (!file_exists(filename))
    {
        printf("File does not exist!\n");
        return 1;
    }

    if (!check_csv_extension(filename))
    {
        printf("File is not a CSV file!\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open file!\n");
        return 1;
    }

    GHashTable *df_data = g_hash_table_new(g_str_hash, g_str_equal);

    char *line = NULL;
    size_t len = 0;
    bool has_header = true;
    GPtrArray *header_array = NULL;

    if (has_header)
    {
        header_array = g_ptr_array_new_with_free_func(g_free);
        getline(&line, &len, file);
        get_headers(line, df_data, header_array);
        free(line);
        line = NULL;
    }

    for (size_t i = 0; i < header_array->len; i++)
    {
        printf("%s\n", (char *)g_ptr_array_index(header_array, i));
    }

    g_hash_table_destroy(df_data);
    g_ptr_array_free(header_array, TRUE);
    fclose(file);
    return 0;
}
