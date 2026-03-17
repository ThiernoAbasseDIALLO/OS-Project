/**
 * @file fifo.h
 * @brief Algorithme d'ordonnancement FIFO (First In, First Out)
 *
 * L'ordonnancement FIFO (aussi appelé FCFS - First Come, First Served)
 * exécute les processus dans l'ordre de leur arrivée dans la file des prêts.
 * C'est un algorithme non-préemptif : un processus garde le CPU jusqu'à
 * la fin de son burst CPU courant.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef FIFO_H
#define FIFO_H

#include "process.h"

/**
 * @brief Sélectionne le prochain processus à exécuter selon la politique FIFO.
 *
 * Retourne le premier processus de la file des prêts (index 0),
 * c'est-à-dire celui qui attend depuis le plus longtemps.
 *
 * @param file_prets  Tableau de pointeurs vers les processus prêts.
 * @param taille      Nombre de processus dans la file.
 * @return Pointeur vers le processus sélectionné, ou NULL si la file est vide.
 */
processus_t *ordonnanceur_fifo(processus_t **file_prets, int taille);

/**
 * @brief Lance la simulation complète avec l'algorithme FIFO.
 *
 * Fait avancer un horloge globale, gère les arrivées, les E/S parallélisées,
 * calcule les indicateurs (attente, réponse, restitution) et exporte les
 * résultats dans un fichier CSV "resultats_fifo.csv".
 *
 * @param liste Tableau de processus à simuler (modifié en place).
 * @param n     Nombre de processus.
 */
void simuler_fifo(processus_t *liste, int n);

#endif /* FIFO_H */
