#ifndef MATRICE_H
#define MATRICE_H

#include "process.h"
#include "tableau.h"

/**
 * @brief Alloue dynamiquement une matrice d'états de processus.
 * * La matrice est typiquement utilisée pour le diagramme de Gantt :
 * - Lignes : Processus (indexés de 0 à nbLig-1).
 * - Colonnes : Temps/Ticks (indexés de 0 à nbCol-1).
 * @param[in] nbLig Nombre de lignes (nombre de processus).
 * @param[in] nbCol Nombre de colonnes (durée totale de la simulation).
 * @return Un pointeur vers le tableau de pointeurs, ou NULL en cas d'échec.
 */
etat_processus_t** allocMat(int nbLig, int nbCol);

/**
 * @brief Libère proprement la mémoire occupée par une matrice.
 * @param[in] nbLig Nombre de lignes de la matrice.
 * @param[in, out] mat Pointeur de la matrice à libérer.
 * @return NULL après libération pour éviter les pointeurs fous.
 */
etat_processus_t** libMat(int nbLig, etat_processus_t** mat);

/**
 * @brief Initialise toutes les cases d'une matrice à l'état par défaut.
 * @param[in] nbCol Nombre de colonnes.
 * @param[in] nbLig Nombre de lignes.
 * @param[out] mat  La matrice déjà allouée à remplir.
 */
void initMat(int nbCol, int nbLig, etat_processus_t** mat);

#endif