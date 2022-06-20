#include <stdlib.h>

#include "chef.h"
#include "config.h"
#include "buffet.h"
#include "globals.h"

void *chef_run()
{
    while (TRUE)
    {
        buffet_t *buffets = globals_get_buffets();
        chef_check_food(buffets);
        //chef_put_food(buffets);
         /* Pode retirar este sleep quando implementar a solução! */
    }

    
    pthread_exit(NULL);
}


void chef_put_food(buffet_t *buffets, int buffet, int meal)
{
    pthread_mutex_lock(&buffets[buffet].mutex_meal[meal]);
    buffets[buffet]._meal[meal] = 40;
    for (int i = 0; i < 40; i++) {
        sem_post(&buffets[buffet].sem_meal[meal]);
    }
    pthread_mutex_unlock(&buffets[buffet].mutex_meal[meal]);
    

    printf("Foi colocada comida %d no Buffet %d\n", meal, buffet);
}
void chef_check_food(buffet_t *buffets)
{
    for (int i = 0; i < globals_get_n_buffets(); i++){
        for (int j = 0; j < 5; j++){
            if (buffets[i]._meal[j] == 0){
                printf("Está faltando comida %d no Buffet %d\n", j, i);
                chef_put_food(buffets, i, j);
            }
        }
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void chef_init(chef_t *self)
{
    pthread_create(&self->thread, NULL, chef_run, NULL);
}

void chef_finalize(chef_t *self)
{
    pthread_join(self->thread, NULL);
    free(self);
}