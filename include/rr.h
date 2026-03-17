/**
 * @file rr.h
 * @brief Algorithme d'ordonnancement RR (Round Robin).
 *
 * Le Round Robin est un algorithme PRÉEMPTIF basé sur un quantum de temps :
 *   - Chaque processus reçoit le CPU pour au maximum `quantum` ms.
 *   - À l'expiration du quantum, le processus est interrompu et remis en
 *     fin de file des prêts (politique FIFO circulaire).
 *   - Un processus qui termine son burst CPU avant la fin du quantum
 *     libère le CPU immédiatement.
 *   - Les E/S sont parallélisées : un processus en attente d'E/S ne
 *     consomme pas de quantum ; quand ses E/S se terminent, il rejoint
 *     la fin de la file des prêts.
 *
 * Le quantum doit être passé en argument à `simuler_rr`.
 * Un quantum de 1 ms est équivalent au SJRF avec égalité de burst ;
 * un quantum très grand tend vers FIFO.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef RR_H
#define RR_H

#include "process.h"

/**
 * @brief Simule l'ordonnancement Round Robin.
 *
 * @param liste  Tableau des processus à ordonnancer (modifié en place).
 * @param n      Nombre de processus.
 * @param quantum Durée maximale du quantum de temps (en ms, > 0).
 */
void simuler_rr(processus_t *liste, int n, int quantum);

#endif /* RR_H */
