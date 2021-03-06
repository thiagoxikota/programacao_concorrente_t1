#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "queue.h"
#include "table.h"
#include "buffet.h"

/**
 * @brief Inicia uma fila (de modo global)
 * 
 * @param queue 
 */
extern void globals_set_queue(queue_t *queue);

/**
 * @brief Retorna uma fila (de modo global)
 * 
 * @return queue_t* 
 */
extern queue_t *globals_get_queue();

/**
 * @brief Insere o número de alunos (de modo global)
 * 
 */
extern void globals_set_students(int number);

/**
 * @brief Retorna o número de alunos (de modo global)
 * 
 * @return int 
 */

extern int globals_get_students();

/**
 * @brief Insere o número de buffets (de modo global)
 * 
 */
extern void globals_set_n_buffets(int number);

/**
 * @brief Retorna o número de buffets (de modo global)
 * 
 * @return int 
 */

extern int globals_get_n_buffets();

/**
 * @brief Inicia um array de mesas (de modo global).
 * 
 * @param t 
 */
extern void globals_set_table(table_t *t);

/**
 * @brief Retorna um array de mesas (de modo global)
 * 
 * @return table_t* 
 */
extern table_t *globals_get_table();

/**
 * @brief Insere o numero de mesas (global)
 * 
 */
extern void globals_set_tables(int n);

/**
 * @brief Retorna o numero de mesas (global)
 * 
 * @return int
 */
extern int globals_get_tables();

/**
 * @brief Insere o numero de acentos (global)
 * 
 */
extern void globals_set_seats(int n);

/**
 * @brief Retorna o numero de acentos (global)
 * 
 * @return int 
 */
extern int globals_get_seats();


/**
 * @brief Finaliza todas as variáveis globais.
 * 
 */
extern void globals_finalize();

/**
 * @brief Inicia um array de buffets (de modo global)
 * 
 */
extern void globals_set_buffets(buffet_t *buffets_ref);

/**
 * @brief Retorna um array de buffets (de modo global)
 * 
 * @return buffet_t* 
 */
extern buffet_t *globals_get_buffets();




#endif