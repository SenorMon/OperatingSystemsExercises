#define BUFFER_SIZE 10
#define STRING_SIZE 50
#define SHM_NAME "/circ_buff"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"

#define MUTEX "/mutex"

typedef enum {
    RUNNING, 
    STOPPED
} state;

typedef struct shm_data{
    state buffer_state;
    int writer_pos;

    int buffer[BUFFER_SIZE];
} shm_data;