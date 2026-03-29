#ifndef TABLEAU_H
#define TABLEAU_H

#include<stdio.h>
#include<stdlib.h>

typedef int TElement;

/* Allocation d'un tableau de TElement */
TElement* allocTab(int nMax);
void allocTabP(int nMax, TElement **tab);

/* Libération d'un tableau de TElement */
TElement* libTab(TElement *tab);
void libTabP(TElement **tab);

#endif 
