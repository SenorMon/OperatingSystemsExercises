#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "data_definition.h"

int delay = -1;
int limit = -1;

sem_t* empty;
sem_t* full;

int reader_pos = 0;

int best_solution_removed_edges = -1;


int main(int argc, char **argv){
    int opt;
    while((opt = getopt(argc, argv, "n:w:")) != -1){
        switch(opt){
            case 'n':
                if(limit == -1){
                    limit = strtoll(optarg, NULL, 10);
                    if(errno == ERANGE){
                        fprintf(stderr, "Something went wrong while parsing the int: %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }else{
                    fprintf(stderr, "There was already a limit set!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'w':
                if(delay == -1){
                    delay = strtoll(optarg, NULL, 10);
                    if(errno == ERANGE){
                        fprintf(stderr, "Something went wrong while parsing the int: %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                }else{
                    fprintf(stderr, "There was already a limit set!\n");
                    exit(EXIT_FAILURE);
                }

                break;
            
                case '?':
                    fprintf(stderr, "Wrong usage! %s [-n limit] [-w delay]", argv[0]);
                    exit(EXIT_FAILURE); 
        }
    }

    delay = (delay < 0) ? 0 : delay;
    int unlimited = (limit == -1) ? 1 : 0;

    //Creating the shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
    if(shm_fd == -1){
        fprintf(stderr, "Couldn't open the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(ftruncate(shm_fd, sizeof(struct shm_data)) == -1){
        fprintf(stderr, "Couldn't truncate the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    shm_data *data;
    data = mmap(NULL, sizeof(*data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(data == MAP_FAILED){
        fprintf(stderr, "Couldn't map the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    } 

    data->buffer_state = RUNNING;
    data->writer_pos = 0;

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

    sleep(delay);


    //Add critical section
    while(limit > 0 || unlimited){
        
        sem_wait(full);
            if(best_solution_removed_edges == -1){
                best_solution_removed_edges = data->buffer[reader_pos];
            }


            if(best_solution_removed_edges > data->buffer[reader_pos]){
                best_solution_removed_edges = data->buffer[reader_pos];
            }

            printf("Possible solution at limit %d: %d\n", limit, data->buffer[reader_pos]);


            reader_pos = (reader_pos + 1) % BUFFER_SIZE;

        sem_post(empty);

        if(best_solution_removed_edges == 0){
            break;
        }


        if(!unlimited){
            limit--;
        }
    }
    
    data->buffer_state = STOPPED;

    if(best_solution_removed_edges == 0){
        printf("The graph is 3-colorable!\n");
    }else{
        printf("The best found solution removes %d edges!\n", best_solution_removed_edges);
    }

    if(munmap(data, sizeof(*data)) == -1){
        fprintf(stderr, "Something went wrong while unmapping the shared memory data: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(shm_unlink(SHM_NAME) == -1){
        fprintf(stderr, "Something went wrong while unlinking the shared memory: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(close(shm_fd) == -1){
        fprintf(stderr, "Something went wrong while closing the file descriptor: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    sem_close(empty);
    sem_close(full);

    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);

    return EXIT_SUCCESS;
}