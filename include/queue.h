/**
 * @file queue.h
 * @brief File des processus prêts basée sur un tableau dynamique.
 *
 * Cette implémentation expose une `file_t` (tableau de pointeurs) qui
 * permet de retirer un élément à un indice quelconque, nécessaire pour
 * les algorithmes SJF/SJRF qui ne servent pas forcément la tête de file.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

/**
 * @brief File des processus prêts (tableau de pointeurs).
 */
typedef struct {
    processus_t **elements; /**< Tableau de pointeurs vers les processus */
    int taille;             /**< Nombre d'éléments actuellement dans la file */
    int capacite;           /**< Capacité maximale allouée */
} file_t;

/**
 * @brief Alloue et initialise une file vide.
 * @param capacite Nombre maximal de processus que la file peut contenir.
 * @return Pointeur vers la file allouée.
 */
file_t *creer_file(int capacite);

/**
 * @brief Libère la mémoire d'une file.
 * @param f Pointeur vers la file à libérer.
 */
void liberer_file(file_t *f);

/**
 * @brief Teste si la file est vide.
 * @param f Pointeur vers la file.
 * @return true si vide, false sinon.
 */
bool estVide(const file_t *f);

/**
 * @brief Ajoute un processus en fin de file.
 * @param f Pointeur vers la file.
 * @param p Pointeur vers le processus à ajouter.
 */
void enfiler(file_t *f, processus_t *p);

/**
 * @brief Retire le processus à l'indice donné et décale les suivants.
 *
 * Indice 0 correspond à la tête de file (FIFO).
 * Un indice quelconque est nécessaire pour SJF/SJRF.
 *
 * @param f      Pointeur vers la file.
 * @param indice Position du processus à retirer (0 = tête).
 * @return Pointeur vers le processus retiré, NULL si indice invalide.
 */
processus_t *defiler(file_t *f, int indice);

#endif /* QUEUE_H */
