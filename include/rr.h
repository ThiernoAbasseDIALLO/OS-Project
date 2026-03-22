/**
 * @file rr.h
 * @brief Algorithme d'ordonnancement Round Robin (RR), préemptif.
 *
 * Chaque processus reçoit le CPU pour un quantum fixe.
 * À l'expiration du quantum, il est remis en FIN de file des prêts.
 * Les E/S sont parallélisées.
 * Utilise la File de queue.h pour la file des prêts.
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

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
 */
void run_rr(processus_t *processus, int n, int quantum, resultats_t *resultats);

#endif /* RR_H */
