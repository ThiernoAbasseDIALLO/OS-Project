#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

/*
 * File représentant la file des processus prêts.
 * Structure simple basée sur un tableau dynamique.
 */

typedef struct {
    processus_t **elements;  // Tableau de pointeurs vers processus
    int capacite;            // Capacité maximale
    int taille;              // Nombre actuel d’éléments
} file_t;


/* Création d’une file */
file_t* creer_file(int capacite);

/* Ajout en fin de file */
void enfiler(file_t *f, processus_t *p);

/* Suppression à un indice donné  */
processus_t* defiler(file_t *f, int indice);

/* Libération mémoire */
void liberer_file(file_t *f);

#endif
