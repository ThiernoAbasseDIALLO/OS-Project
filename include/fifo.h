/**
 * @file fifo.h
 * @brief Algorithme d'ordonnancement FIFO (First In, First Out).
 *
 * Le premier processus arrivé est le premier servi. Non-préemptif.
 * Boucle tick par tick identique à sjrf.c.
 * Utilise la File de queue.h pour la file des prêts.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef FIFO_H
#define FIFO_H

#include "process.h"

/**
 * @brief Exécute l'algorithme FIFO tick par tick.
 *
 * Calcule tous les indicateurs de performance (temps_attente,
 * temps_reponse, temps_restitution, taux_occupation) et les
 * stocke dans resultats.
 * Pas d'affichage ni d'export — géré dans main.c.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param resultats Pointeur vers la structure de résultats à remplir.
 * @param gantt
 */
void run_fifo(processus_t *processus, int n, resultats_t *resultats, etat_processus_t** gantt);

#endif /* FIFO_H */
