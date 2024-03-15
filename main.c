#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100
#define IN_MAT1 "a"
#define IN_MAT2 "b"
#define MAT_OUT "c"
#define MATRIX_TH "_per_matrix"
#define ROW_TH "_per_row"
#define ELEMENT_TH "_per_element"

char matrices[MAX_SIZE];

struct matrixStructure {
    int** mat;
    int r;
    int c;
};

struct productStructure {
    int** matOut;
    int r;
    int c;
    struct matrixStructure *matIn;
};

char** parse_string(char arr[]) {
    char** tokens = (char**)malloc(sizeof(char*));
    char* token = strtok(arr, " ");
    int i = 0;
    while (token != NULL) {
        int len = strlen(token);
        if (token[len - 1] == '\n') token[len - 1] = '\0';
        tokens[i++] = token;
        tokens = (char**)realloc(tokens, (i + 1) * sizeof(char*));
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return tokens;
}

void print_strings(char** string) {
    char** ptr = string;
    while (*ptr != NULL) {
        printf("%s ", *ptr);
        ptr++;
    }
    printf("\n");
}

void read_file(char* file_name, struct matrixStructure *matrix) {
    char path[MAX_SIZE];
    strcpy(path, "../");
    strcat(path, file_name);
    strcat(path, ".txt");

    FILE *fptr;
//    printf(">> %s", path);
    fptr = fopen(path, "r");
    if (fptr == NULL) {
        printf("Error opening file\n");
        exit(0);
    }
    int r, c;
    char param[MAX_SIZE];
    fgets(param, MAX_SIZE, fptr);
//    printf("%s\n", param);
    char** parsed_param = parse_string(param);
    char* row_ptr = strstr(parsed_param[0], "=") + 1;
    r = atoi(row_ptr);
    char* col_ptr = strstr(parsed_param[1], "=") + 1;
    c = atoi(col_ptr);
//    printf(">>> %d %d", r, c);

    int** mat = (int**)malloc(r * sizeof(int*));
    for (int i = 0; i < r; ++i) {
        mat[i] = (int*)malloc(c * sizeof(int));
    }

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            fscanf(fptr, "%d", &mat[i][j]);
        }
    }
    matrix->mat = mat;
    matrix->r = r;
    matrix->c = c;
    fclose(fptr);
}

char** read_matrices() {
    fgets(matrices, MAX_SIZE, stdin);
    char** files = parse_string(matrices);
    return files;
}

void write_file(char* file_name, int **mat) {

}

int main()
{
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    //your code goes here
    char** files = read_matrices();
    struct matrixStructure matA;
    struct matrixStructure matB;

    if (files[1] == NULL) {
        read_file(IN_MAT1, &matA);
        read_file(IN_MAT2, &matB);
    } else {
        read_file(files[1], &matA);
        read_file(files[2], &matB);
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            printf("%d ", matA.mat[i][j]);
        }
        printf("\n");
    }

    free(files);

    gettimeofday(&stop, NULL); //end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    return 0;
}
