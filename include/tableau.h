#ifndef TABLEAU_H
#define TABLEAU_H

#include<stdio.h>
#include<stdlib.h>
#include "process.h"

typedef int TElement;

/* Allocation d'un tableau de TElement */
etat_processus_t* allocTab(int nMax);
void allocTabP(int nMax, etat_processus_t **tab);

/* Libération d'un tableau de TElement */
etat_processus_t* libTab(etat_processus_t *tab);
void libTabP(etat_processus_t **tab);

#endif 
