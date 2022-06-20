#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "worker_gate.h"
#include "globals.h"
#include "config.h"

pthread_mutex_t mutex_buffet_first_position;
pthread_mutex_t mutex_remove_student;



int worker_gate_look_queue() // Verifica se a fila de fora está vazia
{
    queue_t *students_queue = globals_get_queue();
    if (students_queue->_length == 0)
    {
        printf("Fila de fora vazia\n");
        return 1;
    }
    return 0;  
}

void worker_gate_remove_student()
{
    queue_t *q = globals_get_queue();
    msleep(500);
    if (q->_length != 0){
        pthread_mutex_lock(&q->mutex_queue);
        student_t *student = queue_remove(q);
        sem_post(&student->wait_for_gate_worker);
        pthread_mutex_unlock(&q->mutex_queue);
    }
}

void worker_gate_look_buffet()
{
    /* Insira aqui sua lógica */

}

void *worker_gate_run(void *arg)
{
    queue_t *q = globals_get_queue();
    int all_students_entered;
    int number_students;

    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    pthread_mutex_init(&mutex_remove_student, NULL);
    pthread_mutex_init(&mutex_buffet_first_position, NULL);
    sem_init(&q->pode_entrar_em_algum_buffet, 0, globals_get_n_buffets() * 2);

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue();
        worker_gate_look_buffet();
        worker_gate_remove_student();
    }

    pthread_mutex_destroy(&mutex_buffet_first_position);
    pthread_mutex_destroy(&mutex_remove_student);
    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{   
    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

void worker_gate_finalize(worker_gate_t *self)
{   
    queue_t *q = globals_get_queue();

    sem_destroy(&q->pode_entrar_em_algum_buffet);
    pthread_join(self->thread, NULL);
    free(self);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    buffet_t *buffets = globals_get_buffets();

    queue_t *q = globals_get_queue();

    sem_wait(&q->pode_entrar_em_algum_buffet);
    // pthread_mutex_lock(&mutex_buffet_first_position);

    //while para setar quando um estudante esta apto a ser inserido
    //se tiver algum buffet livre
    int n = globals_get_n_buffets();
    for (int i = 0; i < n; i++){

        if (buffets[i].queue_left[0] == 0){
            pthread_mutex_lock(&buffets[i].mutex_queue_left[0]);
            student->_id_buffet = buffets[i]._id;
            student->left_or_right = 'L';
            buffet_queue_insert(buffets,student);

            break;
        }
        if (buffets[i].queue_right[0] == 0){
            pthread_mutex_lock(&buffets[i].mutex_queue_right[0]);
            student->_id_buffet = buffets[i]._id;
            student->left_or_right = 'R';
            buffet_queue_insert(buffets,student);
            break;
        }
    }
}