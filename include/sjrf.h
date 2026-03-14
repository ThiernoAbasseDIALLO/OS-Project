#ifndef SJRF_H
#define SJRF_H

#include "process.h"

/**
 * @brief Lance la simulation de l'algorithme SJRF, qui sélectionne parmi tous les processus prêts, le processus avec le plus court temps de cpu restant et le fait à chaque unité de temps tant que tous les processus ne sont pas terminés.
 * @param processus Tableau des processus à ordonnancer
 * @param n Nombre de processus
 */
void run_sjrf(processus_t *processus, int n);

#endif //SJRF_H