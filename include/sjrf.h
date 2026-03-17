/**
 * @file sjrf.h
 * @brief Algorithme SJRF (Shortest Job Remaining First) — préemptif.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */
#ifndef SJRF_H
#define SJRF_H

#include "process.h"

/**
 * @brief Simule l'ordonnancement SJRF (préemptif, unité de temps = 1 ms).
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 */
void run_sjrf(processus_t *processus, int n, resultats_t *r);

#endif /* SJRF_H */
