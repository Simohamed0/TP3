#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define CHK(op)		do { if ((op) == -1) { raler (#op) ; } } while (0)
#define TCHK(op)	do { if ((errno=(op)) > 0) { raler (#op) ; } } while (0)

/* Message et arrêt */
void raler (char * msg)
{
    perror (msg);
    exit (EXIT_FAILURE);
}

/* Des constantes pour les ingrédients, peu utiles en fait */
#define BEURRE	0
#define FARINE	1
#define OEUFS	2
#define SUCRE	3
#define	NING	4	// nombre d'ingrédients

/* Dans la suite, int[NING] est le type d'un ensemble d'ingrédients,
   soit dans une cuisine, soit sur la table */

/* Mettre à zéro tous les ingrédients */
void zero(int ingr[NING])
{
    ingr[BEURRE] = ingr[FARINE] = ingr[OEUFS] = ingr[SUCRE] = 0;
}

/* Produire une chaîne représentant un ensemble d'ingrédients :
   - pour identifier une cuisinier
   - pour représenter ce qui se trouve sur la table */
void ingstr(const int ingr[NING], char s[NING+1])
{
    s[BEURRE] = '0' + ingr[BEURRE];
    s[FARINE] = '0' + ingr[FARINE];
    s[OEUFS] = '0' + ingr[OEUFS];
    s[SUCRE] = '0' + ingr[SUCRE];
    s[NING] = '\0';
}

/* Combiner deux ensembles d'ingrédients permet-il de faire une gâteau ? */
int suffisant(const int stock[NING], const int table[NING])
{
    return stock[BEURRE] + table[BEURRE] > 0
        && stock[FARINE] + table[FARINE] > 0
        && stock[OEUFS]  + table[OEUFS]  > 0
        && stock[SUCRE]  + table[SUCRE]  > 0;
}

/* Tirer deux ingrédients au hasard */
void fournir(int ingr[NING])
{
    int i1 = rand() % NING;
    int i2 = ((rand() % (NING-1)) + i1 + 1) % NING;
    zero(ingr);
    ingr[i1] = ingr[i2] = 1;
}

/* La table permettant aux threads de communiquer */
struct table {
    int ings[NING]; /* les ingrédients sur la table */
    pthread_mutex_t mutex; /* synchronisation */
    pthread_cond_t cond; /* épicier -> cuisinier */
    sem_t sem; /* cuisinier -> épicier (= gâteau) */
};

/* Argument d'un thread cuisinier */
struct cuisinier_arg {
    const int stock[NING];
    struct table * table;
};

/* Argument d'un thread épicier (pour la symétrie) */
struct epicier_arg {
    struct table * table;
};



/* Un thread cuisinier */
void * cuisinier(void * arg)
{
    struct cuisinier_arg * carg = arg;
    /* ... */
    return NULL;
}

/* Un thread épicier */
void * epicier(void * arg)
{
    struct epicier_arg * earg = arg;
    /* ... */
    return NULL;
}

int main()
{
    /* Initialisation */
    struct table table;
    zero(table.ings);
    TCHK (pthread_mutex_init (&(table.mutex), NULL));
    TCHK (pthread_cond_init (&(table.cond), NULL));
    CHK (sem_init (&(table.sem), 0, 0));

    struct cuisinier_arg c_args[6] = {
        { {0,0,1,1}, &table },
        { {0,1,0,1}, &table },
        { {1,0,0,1}, &table },
        { {0,1,1,0}, &table },
        { {1,0,1,0}, &table },
        { {1,1,0,0}, &table } };
    struct epicier_arg e_arg = { &table };

    /* Creation des threads */
    pthread_t id_cuisiniers[6];
    pthread_t id_epicier;
    for (int i=0 ; i<6 ; i++)
        TCHK (pthread_create (&(id_cuisiniers[i]), NULL,
                              cuisinier, &(c_args[i])));
    TCHK (pthread_create (&id_epicier, NULL, epicier, &e_arg));

    /* Tant qu'il n'y a pas de protocole de terminaison,
       arrêter le programme par Control-C */
    /* ... */

    /* Fin des threads */
    TCHK (pthread_join (id_epicier, NULL));
    for (int i=0 ; i<6 ; i++)
        TCHK (pthread_join (id_cuisiniers[i], NULL));
    TCHK (pthread_mutex_destroy (&(table.mutex)));
    TCHK (pthread_cond_destroy (&(table.cond)));
    CHK (sem_destroy (&(table.sem)));

    return EXIT_SUCCESS;
}

