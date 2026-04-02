#ifndef TABLEAU_H
#define TABLEAU_H

#include<stdio.h>
#include<stdlib.h>
#include "process.h"

typedef int TElement;

/**
 * @brief Alloue un tableau d'états de processus.
 * @param[in] nMax Taille du tableau à allouer.
 * @return Un pointeur vers le tableau alloué, ou NULL en cas d'erreur.
 */
etat_processus_t* allocTab(int nMax);

/**
 * @brief Alloue un tableau via un pointeur de pointeur (passage par adresse).
 * @param[in]  nMax Taille du tableau.
 * @param[in, out] tab  Pointeur vers le pointeur de tableau à initialiser.
 */
void allocTabP(int nMax, etat_processus_t **tab);

/**
 * @brief Libère la mémoire d'un tableau et retourne NULL.
 * @param[in] tab Le tableau a libéré.
 * @return NULL pour permettre la mise à jour du pointeur appelant.
 */
etat_processus_t* libTab(etat_processus_t *tab);

/**
 * @brief Libère la mémoire d'un tableau et met le pointeur à NULL (par adresse).
 * @param[in, out] tab Pointeur vers le pointeur du tableau à libérer.
 */
void libTabP(etat_processus_t **tab);

#endif 
