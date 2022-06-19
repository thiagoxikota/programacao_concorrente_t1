#include <stdlib.h>

#include "worker_gate.h"
#include "globals.h"
#include "config.h"

pthread_mutex_t mutex_gate;

void worker_gate_look_queue()
{
    /* Insira aqui sua lógica */
}

void worker_gate_remove_student()
{
    queue_t *q = globals_get_queue();
    queue_remove(q);

}

void worker_gate_look_buffet()
{
    /* Insira aqui sua lógica */

}

void *worker_gate_run(void *arg)
{
    int all_students_entered;
    int number_students;

    number_students = *((int *)arg);
    all_students_entered = number_students > 0 ? FALSE : TRUE;

    while (all_students_entered == FALSE)
    {
        worker_gate_look_queue();
        worker_gate_look_buffet();
        worker_gate_remove_student();
        msleep(500); /* Pode retirar este sleep quando implementar a solução! */
    }

    pthread_exit(NULL);
}

void worker_gate_init(worker_gate_t *self)
{   

    pthread_mutex_init(&mutex_gate, NULL);

    int number_students = globals_get_students();
    pthread_create(&self->thread, NULL, worker_gate_run, &number_students);
}

void worker_gate_finalize(worker_gate_t *self)
{
    pthread_join(self->thread, NULL);
    pthread_mutex_destroy(&mutex_gate);
    free(self);
}

void worker_gate_insert_queue_buffet(student_t *student)
{
    buffet_t *buffets = globals_get_buffets();

    //while para setar quando um estudante esta apto a ser inserio
    //se tiver algum buffet livre
    int n = globals_get_n_buffets();
    pthread_mutex_lock(&mutex_gate);
    for (int i = 0; i < n; i++){ //Alterar o 2 (config.buffet)
        //mutex
        if (buffets[i].queue_left[0] == 0){
            student->_id_buffet = buffets[i]._id;
            student->left_or_right = 'L';
            buffet_queue_insert(buffets,student);
            break;
        }
        if (buffets[i].queue_right[0] == 0){
            student->_id_buffet = buffets[i]._id;
            student->left_or_right = 'R';
            buffet_queue_insert(buffets,student);
            break;
        }
    }
    // pthread_mutex_unlock(&mutex_gate);
}