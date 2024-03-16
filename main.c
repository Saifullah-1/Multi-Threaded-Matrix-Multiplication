#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256
#define IN_MAT1 "a"
#define IN_MAT2 "b"
#define MAT_OUT "c"
#define MATRIX_TH "_per_matrix"
#define ROW_TH "_per_row"
#define ELEMENT_TH "_per_element"

struct MatrixStructure {
    int (*matrix)[];
    int r;
    int c;
};

struct Data {
    int r;
    int c;
    struct MatrixStructure *matA;
    struct MatrixStructure *matB;
    int (*C)[];
};

void read_file(char* file_name, struct MatrixStructure *matrix) { // Handle file reading errors --> Make fn return int
    char path[MAX_SIZE];
    strcpy(path, file_name);
    strcat(path, ".txt");
    FILE *fptr;
//    printf(">> %s", path);
    fptr = fopen(path, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(0);
    }
    int r, c;
    char param[MAX_SIZE];
    fscanf(fptr, "row=%d col=%d", &r, &c);

    int size = r * c;
    int (*mat)[c] = malloc(r * c * sizeof(int));

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            fscanf(fptr, "%d", &mat[i][j]);
        }
    }
    matrix->matrix = mat;
    matrix->r = r;
    matrix->c = c;
    fclose(fptr);
}

void write_file(char* file_name, char* method_name, struct Data *data) {
    char path[MAX_SIZE];
    strcpy(path, file_name);
    strcat(path, ".txt");
    FILE *fptr;
    fptr = fopen(file_name, "w");
    fprintf(fptr, "%s\n", method_name);
    fprintf(fptr, "row=%d col=%d\n", data->r, data->c);
    for (int i = 0; i < data->r; ++i) {
        for (int j = 0; j < data->c; ++j) {
            fprintf(fptr, "%d ", ((int (*)[data->c]) data->C)[i][j]);
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
    free(data->C);
}

void *compute_matrix(void *st) {
    struct Data *data = st;
    int rows = data->r;
    int cols = data->c;
    int col_row = data->matA->c;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            ((int (*)[cols]) data->C)[i][j] = 0;
            for (int k = 0; k < col_row; ++k) {
                ((int (*)[cols]) data->C)[i][j] += ((int (*)[col_row]) data->matA->matrix)[i][k] * ((int (*)[cols]) data->matB->matrix)[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

void * compute_row(void *st) {
    struct Data *data = st;
    int r = data->r;
    int col_row = data->matA->c;
    for (int i = 0; i < data->c; ++i) {
        ((int (*)[data->c]) data->C)[r][i] = 0;
        for (int j = 0; j < col_row; ++j) {
            ((int (*)[data->c]) data->C)[r][i] += ((int (*)[col_row]) data->matA->matrix)[r][j] * ((int (*)[data->c]) data->matB->matrix)[j][i];
        }
    }
    pthread_exit(NULL);
}

void * compute_element(void *st) {
    struct Data *data = st;
    int r = data->r;
    int c = data->c;
    int col_row = data->matA->c;
    ((int (*)[data->matB->c]) data->C)[r][c] = 0;
    for (int i = 0; i < col_row; ++i) {
        ((int (*)[data->matB->c]) data->C)[r][c] += ((int (*)[col_row]) data->matA->matrix)[r][i] * ((int (*)[data->matB->c]) data->matB->matrix)[i][c];
    }
    pthread_exit(NULL);
}

void matrix_thread(struct Data* data, char* file_name) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, compute_matrix, data) != 0) {
        fprintf(stderr, "ERROR");
        exit(-1);
    }

    pthread_join(thread, NULL);
    strcat(file_name, MATRIX_TH);
    write_file(file_name, "Method: A thread per matrix", data);
}


int row_thread(struct Data* data, char* file_name) {
    int THREADS_NUM = data->r;
    pthread_t threads[THREADS_NUM];
    for (int i = 0; i < THREADS_NUM; ++i) {
        struct Data *st = malloc(sizeof(struct Data));
        st->r = i;
        st->C = data->C;
        st->c = data->c;
        st->matA = data->matA;
        st->matB = data->matB;
        if (pthread_create(&threads[i], NULL, compute_row, st) != 0) {
            fprintf(stderr, "ERROR");
            exit(-1);
        }
    }
    for (int i = 0; i < THREADS_NUM; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "ERROR");
            exit(-1);
        }
    }
    strcat(file_name, ROW_TH);
    write_file(file_name, "Method: A thread per row", data);
    return THREADS_NUM;
}

int element_thread(struct Data* data, char* file_name) {
    int THREADS_NUM = data->r * data->c;
    pthread_t threads[THREADS_NUM];
    int ind = 0;
    for (int i = 0; i < data->r; ++i) {
        for (int j = 0; j < data->c; ++j) {
            struct Data *st = malloc(sizeof(struct Data));
            st->C = data->C;
            st->matA = data->matA;
            st->matB = data->matB;
            st->r = i;
            st->c = j;
            if (pthread_create(&threads[ind++], NULL, compute_element, st) != 0) {
                fprintf(stderr, "ERROR");
                exit(-1);
            }
        }
    }

    for (int i = 0; i < THREADS_NUM; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "ERROR");
            exit(-1);
        }
    }
    strcat(file_name, ELEMENT_TH);
    write_file(file_name, "Method: A thread per element", data);
    return THREADS_NUM;
}

void method_data(char* method_name, int threads_num, struct timeval start, struct timeval stop) {
    printf("Method: %s\n", method_name);
    printf("Number of threads = %d\n", threads_num);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}

int main()
{
    struct MatrixStructure* matA = malloc(sizeof(struct MatrixStructure));
    struct MatrixStructure* matB = malloc(sizeof(struct MatrixStructure));
    struct Data* data = malloc(sizeof(struct Data));

    char input[MAX_SIZE];
    char input1[64], input2[64], input3[64], input4[64];
    fgets(input, MAX_SIZE, stdin);
//    if (strncmp(input1, "./matMult", 9) != 0) {
//        fprintf(stderr, "Invalid command!\n");
//        return 1;
//    }
    if (sscanf(input, "./matMult %63s %63s %63s", input2, input3, input4) != 3) {
        read_file(IN_MAT1, matA);
        read_file(IN_MAT2, matB);
        strcpy(input4, "c");
    } else {
        read_file(input2, matA);
        read_file(input3, matB);
    }

    if (matA->c != matB->r) {
        fprintf(stderr, "Matrices sizes should be X*Y, Y*Z");
        exit(0);
    }
    data->matA = matA;
    data->matB = matB;
    data->r = matA->r;
    data->c = matB->c;

    int th = 1;
    struct timeval stop, start;

    gettimeofday(&start, NULL);
    data->C = malloc(data->r * data->c * sizeof(int));
    matrix_thread(data, input4);
    gettimeofday(&stop, NULL);
    method_data("A thread per matrix", th, start, stop);

    gettimeofday(&start, NULL);
    data->C = malloc(data->r * data->c * sizeof(int));
    th = row_thread(data, input4);
    gettimeofday(&stop, NULL);
    method_data("A thread per row", th, start, stop);

    gettimeofday(&start, NULL);
    data->C = malloc(data->r * data->c * sizeof(int));
    th = element_thread(data, input4);
    gettimeofday(&stop, NULL);
    printf("Time taken by  Method: A thread per element\n");
    method_data("A thread per row", th, start, stop);

    free(matA);
    free(matB);
    free(data);
    return 0;
}
