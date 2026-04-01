#ifndef FIFO_H
#define FIFO_H

#include "process.h"

/**
 * @brief Exécute l'algorithme FIFO tick par tick.
 *
 * * Le simulateur suit les étapes suivantes à chaque unité de temps :
 * - # **Arrivées** : Les nouveaux processus sont placés en fin de file.
 * - # **E/S** : Mise à jour des processus en attente ; retour en file si un burst CPU suit.
 * - # **Élection** : Si le CPU est libre, le processus en tête de file est élu.
 * - # **Exécution** : Décrémentation du temps CPU et mise à jour du diagramme de Gantt.
 * - # **Statistiques** : Incrémentation du temps d'attente pour les processus en file.
 *
 * @param[in, out]  processus Tableau des processus à ordonnancer.
 * @param[in]       n         Nombre de processus total dans le tableau.
 * @param[out]      resultats Pointeur vers la structure stockant les métriques finales (taux d'occupation, etc.).
 * @param[out]      gantt     Matrice d'états pour la visualisation du diagramme de Gantt
 */
void run_fifo(processus_t *processus, int n, resultats_t *resultats, etat_processus_t** gantt);

#endif /* FIFO_H */
