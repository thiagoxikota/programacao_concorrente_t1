#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "student.h"
#include "config.h"
#include "worker_gate.h"
#include "globals.h"
#include "table.h"

void* student_run(void *arg)
{
    student_t *self = (student_t*) arg;
    table_t *tables  = globals_get_table();

    queue_t *q = globals_get_queue();

    pthread_mutex_lock(&q->mutex_queue);
    queue_insert(q, self);
    printf("Student %d entrou na fila, ultimo %d, primeiro %d\n", self->_id, q->_last->_student->_id, q->_first->_student->_id );
    pthread_mutex_unlock(&q->mutex_queue);

    sem_wait(&self->wait_for_gate_worker);
    worker_gate_insert_queue_buffet(self);
    printf("Student %d entrou na fila do buffet %d\n", self->_id, self->_id_buffet);
    sem_post(&self->wait_for_gate_worker);

    student_serve(self);
    // sleep(rand()%3);
    student_seat(self, tables);
    student_leave(self, tables);

    pthread_exit(NULL);
};

void student_seat(student_t *self, table_t *table)
{
    int aux = 1;
    while(aux) {
        for (int i = 0; i < globals_get_tables(); i++) {
            if (table[i]._empty_seats != 0) {
                table[i]._empty_seats -=1;
                self->_id_table = i;
                aux = 0;
                printf("Student: %d sentou na mesa : %d (Acentos vagos: %d)\n", self->_id, i, table[i]._empty_seats);
                break;
            }
        }
    }


}

void student_serve(student_t *self)
{
    msleep(500);
    buffet_t *buffets = globals_get_buffets();
    int wish = self->_wishes[self->_buffet_position];


    if (wish == 1) {

        sem_wait(&buffets[self->_id_buffet].sem_meal[self->_buffet_position]);
        pthread_mutex_lock(&buffets[self->_id_buffet].mutex_meal[self->_buffet_position]);
        printf("Escrevendo posicao buffet %d\n", self->_buffet_position);
        buffets[self->_id_buffet]._meal[self->_buffet_position] -= 1;
        printf("Student %d se serviu da meal %d\n", self->_id, self->_buffet_position);

        pthread_mutex_unlock(&buffets[self->_id_buffet].mutex_meal[self->_buffet_position]);
    }


    buffet_next_step(&buffets[self->_id_buffet],self);


}

void student_leave(student_t *self, table_t *table)
{
    table[self->_id_table]._empty_seats +=1;
    printf("Student: %d saiu da mesa : %d (Acentos vagos: %d)\n", self->_id, self->_id_table, table[self->_id_table]._empty_seats);

}

/* --------------------------------------------------------- */
/* ATENÇÃO: Não será necessário modificar as funções abaixo! */
/* --------------------------------------------------------- */

student_t *student_init()
{
    
    
    student_t *student = malloc(sizeof(student_t));

    sem_init(&student->wait_for_gate_worker, 0, 0);

    student->_id = rand() % 1000;
    student->_buffet_position = -1;
    int none = TRUE;
    for (int j = 0; j <= 4; j++)
    {
        student->_wishes[j] = _student_choice();
        if(student->_wishes[j] == 1) none = FALSE;
    }

    

    if(none == FALSE){
        /* O estudante só deseja proteína */
        student->_wishes[3] = 1;
    }

    //printa students
    printf(" Student %d, wishes: %d %d %d %d %d\n", student->_id, student->_wishes[0], student->_wishes[1], student->_wishes[2], student->_wishes[3], student->_wishes[4]);

    return student;
};

void student_finalize(student_t *self){
    sem_destroy(&self->wait_for_gate_worker);
    free(self);
};


pthread_t students_come_to_lunch(int number_students)
{
    pthread_t lets_go;
    pthread_create(&lets_go, NULL, _all_they_come, &number_students);
    return lets_go;
}

/**
 * @brief Função (privada) que inicializa as threads dos alunos.
 * 
 * @param arg 
 * @return void* 
 */
void* _all_they_come(void *arg)
{
    int number_students = *((int *)arg);
    
    student_t *students[number_students];

    for (int i = 0; i < number_students; i++)
    {
        students[i] = student_init();                                               /* Estudante é iniciado, recebe um ID e escolhe o que vai comer*/
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_create(&students[i]->thread, NULL, student_run, students[i]);       /*  Cria as threads  */
    }

    for (int i = 0; i < number_students; i++)
    {
        pthread_join(students[i]->thread, NULL);                                    /*  Aguarda o término das threads   */
    }

    for (int i = 0; i < number_students; i++)
    {
        student_finalize(students[i]);                                              /*  Libera a memória de cada estudante  */
    }

    pthread_exit(NULL);
}

/**
 * @brief Função que retorna as escolhas dos alunos, aleatoriamente (50% para cada opção)
 *        retornando 1 (escolhido) 0 (não escolhido). É possível que um aluno não goste de nenhuma opção
 *         de comida. Nesse caso, considere que ele ainda passa pela fila, como todos aqueles que vão comer.
 * @return int 
 */
int _student_choice()
{
    float prob = (float)rand() / RAND_MAX;
    return prob > 0.51 ? 1 : 0;
}