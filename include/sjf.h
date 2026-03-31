/**
 * @file sjf.h
 * @brief Algorithme d'ordonnancement SJF (Shortest Job First), non-préemptif.
 *
 * Parmi tous les processus prêts, on exécute celui dont le burst CPU
 * courant est le plus court. Non-préemptif. Les E/S sont parallélisées.
 * Utilise la File de queue.h pour la file des prêts.
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef SJF_H
#define SJF_H

#include "process.h"

/**
 * @brief Exécute l'algorithme SJF non-préemptif tick par tick.
 *
 * Calcule tous les indicateurs de performance (temps_attente,
 * temps_reponse, temps_restitution, taux_occupation) et les
 * stocke dans resultats.
 * Pas d'affichage ni d'export — géré dans main.c.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param resultats Pointeur vers la structure de résultats à remplir.
 */
void run_sjf(processus_t *processus, int n, resultats_t *resultats, etat_processus_t **gantt);

#endif /* SJF_H */
