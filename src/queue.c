/**
 * @file queue.c
 * @brief Implémentation de la file des processus prêts (tableau dynamique).
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

/**
 * @brief Alloue et initialise une file vide de capacité donnée.
 */
file_t *creer_file(int capacite)
{
    file_t *f = malloc(sizeof(file_t));
    if (!f) { perror("malloc file_t"); exit(1); }

    f->elements = malloc(sizeof(processus_t *) * capacite);
    if (!f->elements) { perror("malloc elements"); exit(1); }

    f->taille   = 0;
    f->capacite = capacite;
    return f;
}

/**
 * @brief Libère la mémoire d'une file (pas les processus eux-mêmes).
 */
void liberer_file(file_t *f)
{
    if (!f) return;
    free(f->elements);
    free(f);
}

/**
 * @brief Retourne true si la file ne contient aucun processus.
 */
bool estVide(const file_t *f)
{
    return f == NULL || f->taille == 0;
}

/**
 * @brief Ajoute un processus en fin de file.
 */
void enfiler(file_t *f, processus_t *p)
{
    if (f->taille >= f->capacite) {
        fprintf(stderr, "enfiler : file pleine (capacite=%d)\n", f->capacite);
        return;
    }
    f->elements[f->taille++] = p;
}

/**
 * @brief Retire le processus à l'indice donné, décale les suivants vers la gauche.
 *
 * - defiler(f, 0) → comportement FIFO classique (retire la tête).
 * - defiler(f, i) → retire un élément quelconque (utile pour SJF/SJRF).
 */
processus_t *defiler(file_t *f, int indice)
{
    if (!f || indice < 0 || indice >= f->taille) return NULL;

    processus_t *p = f->elements[indice];

    /* Décalage vers la gauche pour combler le trou */
    for (int i = indice; i < f->taille - 1; i++)
        f->elements[i] = f->elements[i + 1];

    f->taille--;
    return p;
}
