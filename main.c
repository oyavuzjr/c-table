#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <glib.h>

typedef struct DataFrame {
    GHashTable *df_data;
    GPtrArray *headers;
    size_t size;
} DataFrame;

DataFrame *data_frame_new() {
    DataFrame *df = malloc(sizeof(DataFrame));
    df->df_data = g_hash_table_new(g_str_hash, g_str_equal);
    df->headers = g_ptr_array_new_with_free_func(g_free);
    df->size = 0;
    return df;
}

void data_frame_free(DataFrame *df) {
    if (!df) return;
    g_hash_table_destroy(df->df_data);
    g_ptr_array_free(df->headers, TRUE);
    free(df);
}

void get_headers(char *line, DataFrame *df) {
    size_t len = strlen(line);
    char *current_header = line;

    for (size_t i = 0; i < len; ++i) {
        if (line[i] == ',' || line[i] == '\n' || line[i] == '\0') {
            line[i] = '\0';
            g_ptr_array_add(df->headers, g_strdup(current_header));
            GPtrArray *column_array = g_ptr_array_new_with_free_func(g_free);
            g_hash_table_insert(df->df_data, g_strdup(current_header), column_array);
            current_header = line + i + 1;
        }
    }
}

void process_line(char *line, DataFrame *df) {
    size_t len = strlen(line);
    char *current_cell = line;
    size_t index = 0;
    for (size_t i = 0; i < len; i++) {
        if (line[i] == ',' || line[i] == '\0') {
            line[i] = '\0';
            char* key = (char*)g_ptr_array_index(df->headers, index);
            GPtrArray *arr = g_hash_table_lookup(df->df_data, key);
            g_ptr_array_add(arr, g_strdup(current_cell));
            current_cell = line + i + 1;
            index++;
        }
    }
    df->size++;  // Update size
}


bool file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool check_csv_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return false;
    return strcmp(dot, ".csv") == 0;
}


void print_df(DataFrame *data_frame){
    for (size_t row = 0; row < data_frame->size; row++) {
        for (size_t col = 0; col < data_frame->headers->len; col++) {
            char *header = (char *)g_ptr_array_index(data_frame->headers, col);
            GPtrArray *column = (GPtrArray *)g_hash_table_lookup(data_frame->df_data, header);
            printf("%s", (char *)g_ptr_array_index(column, row));
            if (col < data_frame->headers->len - 1)
                printf(",");
        }
        printf("\n");
    }
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Please provide exactly one argument: the path to the CSV file\n");
        return 1;
    }

    char *filename = argv[1];

    if (!file_exists(filename)) {
        printf("File does not exist!\n");
        return 1;
    }

    if (!check_csv_extension(filename)) {
        printf("File is not a CSV file!\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file!\n");
        return 1;
    }

    DataFrame *df = data_frame_new();

    char *line = NULL;
    size_t len = 0;

    getline(&line, &len, file);  // assuming you handle the returned value from getline appropriately in the real code
    get_headers(line, df);       // you need to refactor get_headers to work with DataFrame
    free(line);
    line = NULL;

    while (getline(&line, &len, file) != -1) {
        process_line(line, df);   // you need to refactor process_line to work with DataFrame
        free(line);
        line = NULL;
    }

    print_df(df);

    // At this point df contains all your CSV data.
    // Add any further processing you want on 'df' here.

    data_frame_free(df);  // clean up DataFrame
    fclose(file);  // close the file

    return 0;
}
