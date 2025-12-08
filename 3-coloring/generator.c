#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

#include "data_definition.h"


sem_t* empty;
sem_t* full;
sem_t* mutex;


int get_vertex_count(int argc, char **argv){
    int max = -1;

    for(int i = 1; i < argc; i++){
        int edge1, edge2;
        sscanf(argv[i], "%d-%d", &edge1, &edge2);

        if(edge1 > edge2){
            max = (max < edge1) ? edge1 : max;
        }else{
            max = (max < edge2) ? edge2 : max;
        }
    }

    return max + 1;
}

void clear_matrix(int vertex_count, int **matrix){
    for(int i = 0; i < vertex_count; i++){
        for(int j = 0; j < vertex_count; j++){
            matrix[i][j] = 0;
        }
    }
}

void free_matrix(int vertex_count, int  **matrix){
    for(int i = 0; i < vertex_count; i++){
        free(matrix[i]);
    }

    free(matrix);
}

void generate_edge_colors(int vertex_count, char *edge_colors){
  

    // Schritt 1: Alle zufällig initialisieren
    for (int i = 0; i < vertex_count; i++) {
        int r = rand() % 3;
        if (r == 0) edge_colors[i] = 'r';
        else if (r == 1) edge_colors[i] = 'b';
        else edge_colors[i] = 'g';
    }

    
    int has_r = 0, has_b = 0, has_g = 0;

    for (int i = 0; i < vertex_count; i++) {
        if (edge_colors[i] == 'r') has_r = 1;
        if (edge_colors[i] == 'b') has_b = 1;
        if (edge_colors[i] == 'g') has_g = 1;
    }

    if (!has_r && vertex_count > 0)
        edge_colors[rand() % vertex_count] = 'r';

    if (!has_b && vertex_count > 0)
        edge_colors[rand() % vertex_count] = 'b';

    if (!has_g && vertex_count > 0)
        edge_colors[rand() % vertex_count] = 'g';
}

void generate_matrix(int argc, char **argv, int **matrix){
    int vertex_count = get_vertex_count(argc, argv);

    for(int i = 0; i < vertex_count; i++){
         for (int j = 0; j < vertex_count; ++j) matrix[i][j] = 0;
    }

    for(int i = 1; i < argc; i++){
        int edge1, edge2;
        sscanf(argv[i], "%d-%d", &edge1, &edge2);

        matrix[edge1][edge2] = 1;
        matrix[edge2][edge1] = 1;
    }

}

void remove_color_matrix(int vertex_count, int* removed_edges_ptr, char* edge_colors, int **matrix){
    for(int i = 0; i < vertex_count; i++){
        for(int j = i + 1; j < vertex_count; j++){
            if(matrix[i][j] == 1 && edge_colors[i] == edge_colors[j]){
                matrix[i][j] = 0;
                matrix[j][i] = 0;
                (*removed_edges_ptr)++;
            }
        }
    }
}

char* matrix_to_string(int vertex_count, int **matrix){
    char* edge_list = malloc(STRING_SIZE);
    edge_list[0] = '\0';


    for(int i = 0; i < vertex_count; i++){
        for(int j = i+1; j < vertex_count; j++){
            if(matrix[i][j] == 1){
                char buf[6];
                snprintf(buf, 23, "%d-%d ", i, j);

                strcat(edge_list, buf);
            }
        } 
    }

    return edge_list;
}


void print_matrix(int vertex_count, int **matrix){
    for(int i = 0; i < vertex_count; i++){
        for(int j = 0; j < vertex_count; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv){    
    if(argc < 2){
        fprintf(stderr, "To few arguments! Usage: %s EDGE1 ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0600);
    if(shm_fd == -1){
        fprintf(stderr, "Couldn't open the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    shm_data *data;
    data = mmap(NULL, sizeof(*data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(data == MAP_FAILED){
        fprintf(stderr, "Couldn't map the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
 
    //Initializing the semaphores
    empty = sem_open(SEM_EMPTY, O_CREAT, 0600, BUFFER_SIZE);
    if(empty == SEM_FAILED){
        fprintf(stderr, "Couldn't open the semaphore: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    
    full = sem_open(SEM_FULL, O_CREAT, 0600, 0);
    if(full == SEM_FAILED){
        fprintf(stderr, "Couldn't open the semaphore: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    mutex = sem_open(MUTEX, O_CREAT | O_EXCL, 0600, 1);
    if(mutex == SEM_FAILED){
        mutex = sem_open(MUTEX, O_CREAT);

        if(mutex == SEM_FAILED){
             fprintf(stderr, "Couldn't open the semaphore: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    

    int vertex_count = get_vertex_count(argc, argv);
    char *edge_colors = malloc(vertex_count * sizeof(char));
    int **matrix = malloc(vertex_count * sizeof(int*));
    
    for(int i = 0; i < vertex_count; i++) matrix[i] = malloc(vertex_count * sizeof(int));


    while(data->buffer_state != STOPPED){
        generate_matrix(argc, argv, matrix);
        generate_edge_colors(vertex_count, edge_colors);
        
        int removed_edges = 0;
        remove_color_matrix(vertex_count, &removed_edges, edge_colors, matrix);
        
        sem_wait(empty);
        sem_wait(mutex);

        data->buffer[data->writer_pos] = removed_edges;
        data->writer_pos = (data->writer_pos + 1) % BUFFER_SIZE;

        sem_post(mutex);
        sem_post(full);
        
        clear_matrix(vertex_count, matrix);
    }
    

    if(munmap(data, sizeof(*data)) == -1){
        fprintf(stderr, "Something went wrong while unmapping the shared memory data: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(close(shm_fd) == -1){
        fprintf(stderr, "Something went wrong while closing the file descriptor: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    sem_unlink(MUTEX);

    free_matrix(vertex_count, matrix);
    free(edge_colors);
}