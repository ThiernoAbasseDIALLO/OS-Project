#ifndef MATRICE_H
#define MATRICE_H

#include "process.h"
#include "tableau.h"

etat_processus_t** allocMat(int nbLig, int nbCol);
etat_processus_t** libMat(int nbLig, etat_processus_t** mat);
void initMat(int nbCol, int nbLig, etat_processus_t** mat);

#endif