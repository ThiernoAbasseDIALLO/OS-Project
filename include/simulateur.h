// include/simulateur.h
#ifndef SIMULATEUR_H
#define SIMULATEUR_H

#include "process.h"
#include "scheduler.h"
#include "queue.h"

/* Type de fonction pour un ordonnanceur */
typedef processus_t* (*ordonnanceur_t)(processus_t **file_prets, int taille);

/* Prototype de la fonction de simulation  */
void simuler(processus_t *liste,
             int n,
             ordonnanceur_t ordonnanceur,
             const char *nom_algo);

#endif
