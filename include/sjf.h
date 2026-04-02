#ifndef SJF_H
#define SJF_H

#include "process.h"

/**
 * @brief Exécute l'algorithme SJF non-préemptif tick par tick.
 *
 * Calcule tous les indicateurs de performance (temps_attente,
 * temps_reponse, temps_restitution, taux_occupation) et les
 * stocke dans resultats.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param resultats Pointeur vers la structure de résultats à remplir.
 * @param gantt     Matrice d'états pour la visualisation du diagramme de Gantt
 */
void run_sjf(processus_t *processus, int n, resultats_t *resultats, etat_processus_t **gantt);

#endif /* SJF_H */
