#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

/*
 * Création de la file des prêts
 */
file_t* creer_file(int capacite)
{
    file_t *f = malloc(sizeof(file_t));
    if (!f) {
        perror("Erreur allocation file");
        exit(EXIT_FAILURE);
    }

    f->elements = malloc(sizeof(processus_t*) * capacite);
    if (!f->elements) {
        perror("Erreur allocation elements file");
        exit(EXIT_FAILURE);
    }

    f->capacite = capacite;
    f->taille = 0;

    return f;
}


/*
 * Ajout d’un processus en fin de file
 */
void enfiler(file_t *f, processus_t *p)
{
    if (f->taille >= f->capacite) {
        fprintf(stderr, "Erreur: file pleine\n");
        exit(EXIT_FAILURE);
    }

    f->elements[f->taille++] = p;
}


/*
 * Suppression d’un processus à un indice donné
 */
processus_t* defiler(file_t *f, int indice)
{
    if (indice < 0 || indice >= f->taille)
        return NULL;

    processus_t *p = f->elements[indice];

    for (int i = indice; i < f->taille - 1; i++)
        f->elements[i] = f->elements[i + 1];

    f->taille--;

    return p;
}


/*
 * Libération mémoire
 */
void liberer_file(file_t *f)
{
    if (!f) return;

    free(f->elements);
    free(f);
}
