#include <stdlib.h>
#include "buffet.h"
#include "config.h"
#include "pthread.h"
#include "semaphore.h"
//mexi
#include "globals.h"





void *buffet_run(void *arg)
{   
    int all_students_entered = FALSE;
    buffet_t *self = (buffet_t*) arg;
    // buffet_t *buffets = globals_get_buffets();



    /*  O buffet funciona enquanto houver alunos na fila externa. */
    while (all_students_entered == FALSE)
    {
        /* Cada buffet possui: Arroz, Feijão, Acompanhamento, Proteína e Salada */
        /* Máximo de porções por bacia (40 unidades). */


        // printf("AQUIIIIIIIIIIII %d\n", buffets[0].queue_left[0]);
        // printf("AQUIIIIIIIIIIII %d\n", buffets[0].queue_left[0]);

        _log_buffet(self);





        msleep(100); /* Pode retirar este sleep quando implementar a solução! */


        // queue_t *q = globals_get_queue();
        // if(q->_length == 0) {
        //     all_students_entered = TRUE;
        // }

        // int cont = 0;

        // for (int j = 0; j < 5; j++) {
        //     if ( self->queue_left[j] == 0 && self->queue_right[j] == 0) {
        //         cont++;
        //         // printf("%d\n", cont);
        //     }
        // }

    
        // if (cont == 5) {
        //     all_students_entered = TRUE;
        //     printf("Buffet %d vazio\n", self->_id);
        // }

    }

    pthread_exit(NULL);
}

void buffet_init(buffet_t *self, int number_of_buffets)
{   

    


    int i = 0, j = 0;
    for (i = 0; i < number_of_buffets; i++)
    {
        /*A fila possui um ID*/
        self[i]._id = i;

        /* Inicia com 40 unidades de comida em cada bacia */
        for(j = 0; j < 5; j++)
            self[i]._meal[j] = 40;

        for(j= 0; j< 5; j++){
             /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_left[j] = 0;
            /* A fila esquerda do buffet possui cinco posições. */
            self[i].queue_right[j] = 0;
        }
        

        for (int j = 0; j < 5; j++) {
            pthread_mutex_init(&self[i].mutex_meal[j], NULL);
            pthread_mutex_init(&self[i].mutex_queue_left[j], NULL);
            pthread_mutex_init(&self[i].mutex_queue_right[j], NULL);
            sem_init(&self[i].sem_meal[j], 0, 40);
        }    

        pthread_create(&self[i].thread, NULL, buffet_run, &self[i]);
        
    }


}


int buffet_queue_insert(buffet_t *self, student_t *student)
{
    /* Se o estudante vai para a fila esquerda */
    if (student->left_or_right == 'L') 
    {
        /* Verifica se a primeira posição está vaga */
        if (!self[student->_id_buffet].queue_left[0])
        {
            self[student->_id_buffet].queue_left[0] = student->_id;
            student->_buffet_position = 0;
            return TRUE;
        }
        return FALSE;
    }
    else
    {   /* Se o estudante vai para a fila direita */
        if (!self[student->_id_buffet].queue_right[0])
        {
            /* Verifica se a primeira posição está vaga */
            self[student->_id_buffet].queue_right[0] = student->_id;
            student->_buffet_position = 0;
            return TRUE;
        }
        return FALSE;
    }
}


void buffet_next_step(buffet_t *self, student_t *student)
{   
    msleep(1000);
    queue_t *q = globals_get_queue();
    /* Se estudante ainda precisa se servir de mais alguma coisa... */
    if (student->_buffet_position + 1 < 5)
    {    /* Está na fila esquerda? */
        if (student->left_or_right == 'L')
        {   /* Caminha para a posição seguinte da fila do buffet.*/

            int position = student->_buffet_position;
            printf("%d\n", position);

            //libera o mutex da proxima posicao no buffet

            printf("%d Tentando lockar o proximo\n", student->_id);    
            pthread_mutex_lock(&self[position].mutex_queue_left[position+1]);
            printf("%d Lockei o proximo\n", student->_id); 
            if (student->_buffet_position == 0) {
                sem_post(&q->pode_entrar_em_algum_buffet);
            }
            self[student->_id_buffet].queue_left[position] = 0;
            self[student->_id_buffet].queue_left[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;

            //libera o mutex da posicao atual no buffet
            pthread_mutex_unlock(&self[position].mutex_queue_left[position]);
            printf("%d Deu unlock na posicao %d\n", student->_id, position); 
            student_serve(student);

        }else /* Está na fila direita? */
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            int position = student->_buffet_position;

            //libera o mutex da proxima posicao no buffet
        
           
            pthread_mutex_lock(&self[position].mutex_queue_right[position+1]);
            self[student->_id_buffet].queue_right[position] = 0;
            if (student->_buffet_position == 0) {
                sem_post(&q->pode_entrar_em_algum_buffet);
            }
            self[student->_id_buffet].queue_right[position + 1] = student->_id;
            student->_buffet_position = student->_buffet_position + 1;

            //libera o mutex da posicao atual no buffet
            pthread_mutex_unlock(&self[position].mutex_queue_right[position]);

            student_serve(student);
        }
    } else {
        // else, para o caso de id_buffet = 4, ou seja, o prox passo (next_step) será sair do buffet e ir sentar na mesa (student_seat())
        int position = student->_buffet_position;
         /* Está na fila esquerda? */
        if (student->left_or_right == 'L')
        {   /* Caminha para a posição seguinte da fila do buffet.*/
            
            self[student->_id_buffet].queue_left[4] = 0;
            pthread_mutex_unlock(&self[position].mutex_queue_left[position]);

        }else /* Está na fila direita? */
        {   /* Caminha para a posição seguinte da fila do buffet.*/

            self[student->_id_buffet].queue_right[4] = 0;
            pthread_mutex_unlock(&self[position].mutex_queue_right[position]);

        }
    }
}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

void buffet_finalize(buffet_t *self, int number_of_buffets)
{
    /* Espera as threads se encerrarem...*/
    for (int i = 0; i < number_of_buffets; i++)
    {
        pthread_join(self[i].thread, NULL);
        for (int j = 0; j < 5; j++) {
            pthread_mutex_destroy(&self[i].mutex_meal[j]);
            pthread_mutex_destroy(&self[i].mutex_queue_left[j]);
            pthread_mutex_destroy(&self[i].mutex_queue_right[j]);
            sem_destroy(&self[i].sem_meal[j]);
        }
        
    }



 

    
    /*Libera a memória.*/
    free(self);
}


void _log_buffet(buffet_t *self)
{
    /* Prints do buffet */
    int *ids_left = self->queue_left; 
    int *ids_right = self->queue_right; 

    printf("\n\n\u250F\u2501 Queue left: [ %d %d %d %d %d ]\n", ids_left[0],ids_left[1],ids_left[2],ids_left[3],ids_left[4]);
    fflush(stdout);
    printf("\u2523\u2501 BUFFET %d = [RICE: %d/40 BEANS:%d/40 PLUS:%d/40 PROTEIN:%d/40 SALAD:%d/40]\n",
           self->_id, self->_meal[0], self->_meal[1], self->_meal[2], self->_meal[3], self->_meal[4]);
    fflush(stdout);
    printf("\u2517\u2501 Queue right: [ %d %d %d %d %d ]\n", ids_right[0],ids_right[1],ids_right[2],ids_right[3],ids_right[4]);
    fflush(stdout);
}