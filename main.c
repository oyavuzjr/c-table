#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>
#include <sys/stat.h> // Needed for checking file existence

void process_line(char *line, GHashTable *df_data, GPtrArray * header_array)
{
    size_t len = strlen(line);
    char *current_cell = line;
    size_t index = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (line[i] == ',' || line[i] == '\0')
        {
            line[i] = '\0';
            char* key = (char*)g_ptr_array_index(header_array,index);
            GPtrArray *arr = g_hash_table_lookup(df_data,key);
            g_ptr_array_add(arr,g_strdup(current_cell));
            current_cell = line + i + 1;
            index++;
            
        }
    }
}

void get_headers(char *line, GHashTable *df_data, GPtrArray *header_array)
{
    size_t len = strlen(line);
    char *current_header = line;

    for (size_t i = 0; i < len; ++i)
    {
        if (line[i] == ',' || line[i] == '\n' || line[i] == '\0')
        {
            line[i] = '\0';
            g_ptr_array_add(header_array, g_strdup(current_header));
            GPtrArray *column_array = g_ptr_array_new_with_free_func(g_free);
            g_hash_table_insert(df_data, g_strdup(current_header), column_array);
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


    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1)
    {
        process_line(line,df_data,header_array);
        free(line);
        line=NULL;
    }

    g_hash_table_destroy(df_data);
    g_ptr_array_free(header_array, TRUE);
    fclose(file);
    return 0;
}
