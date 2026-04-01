/**
 * @file matrice.c
 * @brief Gestion de la matrice pour le diagramme de Gantt.
 * Ce fichier contient les fonctions nécessaires pour créer, initialiser
 * et détruire une matrice de type etat_processus_t.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include "matrice.h"
#include <stdlib.h>

etat_processus_t** allocMat(int nbLig, int nbCol){
	etat_processus_t** mat;

	/* Allocation du tableau de pointeurs (les lignes) */
	mat = (etat_processus_t**)malloc(nbLig*sizeof(etat_processus_t*));
	
	if(mat == NULL){
		perror("Probleme d'allocation");
		return NULL;
	}

	/* Allocation de chaque ligne (les colonnes) via allocTab */
	for(int i=0; i<nbLig; i++)
		mat[i] = allocTab(nbCol);
		//allocTabP(nbCol, &mat[i]);
		
	return mat;
}

etat_processus_t** libMat(int nbLig, etat_processus_t** mat){
	if (mat == NULL) return NULL;

	/* Libération de chaque ligne individuellement */
	for(int i=0; i<nbLig; i++){
		mat[i] = libTab(mat[i]);
	}

	/* Libération du tableau de pointeurs principal */
	free(mat);
	mat = NULL;

	return mat;
}

void initMat(int nbCol, int nbLig, etat_processus_t** mat) {
	if (mat == NULL)
		return;

	/* Deux boucles imbriquées pour initialiser à l'état NOUVEAU. */
	for (int i=0; i<nbLig; i++) {
		for (int j=0; j<nbCol; j++) {
			mat[i][j] = ETAT_NOUVEAU;
		}
	}
}