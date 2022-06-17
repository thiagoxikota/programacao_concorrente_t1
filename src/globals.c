#include <stdlib.h>
#include "globals.h"

queue_t *students_queue = NULL;
table_t *table = NULL;
buffet_t *buffets_ref = NULL;
int n_tables = 0;
int n_seats = 0;

int students_number = 0;

void globals_set_queue(queue_t *queue)
{
    students_queue = queue;
}

queue_t *globals_get_queue()
{
    return students_queue;
}

void globals_set_table(table_t *t)
{
    table = t;
}

void globals_set_n_tables(int n) {
    n_tables = n;
}

int globals_get_n_tables()
{
    return n_tables;
}

void globals_set_n_seats(int n) {
    n_seats = n;
}

int globals_get_n_seats()
{
    return n_seats;
}

table_t *globals_get_table()
{
    return table;
}


void globals_set_students(int number)
{
    students_number = number;
}

int globals_get_students()
{
    return students_number;
}

void globals_set_buffets(buffet_t *buffets)
{
    buffets_ref = buffets;
}

buffet_t *globals_get_buffets()
{
    return buffets_ref;
}


/**
 * @brief Finaliza todas as variáveis globais que ainda não foram liberadas.
 *  Se criar alguma variável global que faça uso de mallocs, lembre-se sempre de usar o free dentro
 * dessa função.
 */
void globals_finalize()
{
    free(table);
}