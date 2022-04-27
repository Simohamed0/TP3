#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            raler(1, #op); \
    } while (0)

#define TCHK(op)                \
    do {                        \
        if ((errno = (op)) > 0) \
            raler(1, #op);      \
    } while (0)

noreturn void raler(int syserr, const char *msg, ...) {
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}

unsigned int engines = 0;

struct thread_info {
    
    int p ;
    int m;
    pthread_mutex_t mutex;
    pthread_cond_t  condition;

};

void *thread_func(void *arg) {
    
    struct thread_info *info = arg;
    int p = info->p;
    int m = info->m;
    pthread_mutex_t mutex = info->mutex;
    pthread_cond_t  condition = info->condition;
    

    int k ;


    for (int j = 0; j < p ; j++)
    {
        
        unsigned int seed = j*7;
        k = rand_r(&seed) % m + 1;  // rand is not thread safe !!!!

        TCHK(pthread_mutex_lock(&mutex));
        while ( m < k )
        {
            TCHK(pthread_cond_wait(&condition, &mutex));
        }
        engines -= k;
        TCHK(pthread_mutex_unlock(&mutex));
        
    
        int k = rand_r(&seed) % 3 + 1;  // work simulation
        sleep(k);
        

        TCHK(pthread_mutex_lock(&mutex));
        
        TCHK(pthread_cond_broadcast(&condition));
        engines += k;

        TCHK(pthread_mutex_unlock(&mutex));
    }
    printf("thread start the job\n" );

    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 4) 
    {
        raler(0, "usage: %s <m> <n> <p>", argv[0]);
    }

    int m = atoi(argv[1]), n = atoi(argv[2]), p = atoi(argv[3]);
    if (n <= 0 || p <= 0 || m <= 0) 
    {
        raler(0, "m ,n and p must be positive");
    }
    engines = m;

    pthread_mutex_t mutex ;
    pthread_cond_t condition ;

    TCHK(pthread_mutex_init(&mutex, NULL));
    TCHK(pthread_cond_init(&condition, NULL));
 
    struct thread_info *info = calloc(n, sizeof(struct thread_info));
    pthread_t* tid = calloc(n, sizeof(pthread_t));
    
    if (info == NULL ) 
    {
        raler(1, "calloc on info");
    }
    
    for (int i = 0; i < n; i++) 
    {            
        info->p = p;
        info->m = m;
        info->mutex = mutex ;
        info->condition = condition;
        
        TCHK(pthread_create(&tid[i], NULL, thread_func, &info[i]));
    }

    for (int i = 0; i < n; i++) 
    {
        TCHK(pthread_join(tid[i], NULL));
    }
    
    free(info);
    return 0;
}