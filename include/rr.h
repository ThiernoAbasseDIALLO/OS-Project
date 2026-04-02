#ifndef RR_H
#define RR_H

#include "process.h"

/**
 * @brief Exécute l'algorithme Round Robin tick par tick.
 *
 * Calcule tous les indicateurs de performance (temps_attente,
 * temps_reponse, temps_restitution, taux_occupation) et les
 * stocke dans resultats.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param quantum   Durée du quantum en ms (doit être > 0).
 * @param resultats Pointeur vers la structure de résultats à remplir.
 * @param gantt     Matrice d'états pour la visualisation du diagramme de Gantt
 */
void run_rr(processus_t *processus, int n, int quantum, resultats_t *resultats, etat_processus_t **gantt);

#endif /* RR_H */
