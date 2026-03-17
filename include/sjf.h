/**
 * @file sjf.h
 * @brief Algorithme d'ordonnancement SJF (Shortest Job First), non-préemptif.
 *
 * Le SJF choisit, parmi tous les processus prêts, celui dont le burst CPU
 * courant est le plus court. En cas d'égalité, le premier arrivé est
 * favorisé (tie-breaking FIFO).
 *
 * C'est un algorithme non-préemptif : une fois lancé, le processus
 * s'exécute jusqu'à la fin de son burst CPU, même si un processus plus
 * court arrive entre-temps (contrairement à SJRF/SRTF).
 *
 * Le SJF est optimal pour minimiser le temps d'attente moyen
 * dans un contexte de processus tous disponibles à t=0.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#ifndef SJF_H
#define SJF_H

#include "process.h"

/**
 * @brief Sélectionne le processus au burst CPU le plus court (SJF).
 *
 * Parcourt la file des prêts et retourne le processus dont
 * `temps_cpu_restant` est minimal. En cas d'égalité, le processus
 * à l'indice le plus bas (arrivé en premier dans la file) est choisi.
 *
 * @param file_prets  Tableau de pointeurs vers les processus prêts.
 * @param taille      Nombre de processus dans la file.
 * @return Pointeur vers le processus sélectionné, ou NULL si la file est vide.
 */
processus_t *ordonnanceur_sjf(processus_t **file_prets, int taille);

/**
 * @brief Lance la simulation complète avec l'algorithme SJF non-préemptif.
 *
 * Boucle événementielle identique à FIFO mais la sélection du prochain
 * processus à exécuter se fait via `ordonnanceur_sjf` au lieu de prendre
 * simplement la tête de file.
 *
 * Les résultats sont exportés dans "resultats_sjf.csv".
 *
 * @param liste Tableau de processus à simuler (modifié en place).
 * @param n     Nombre de processus.
 */
void simuler_sjf(processus_t *liste, int n);

#endif /* SJF_H */
