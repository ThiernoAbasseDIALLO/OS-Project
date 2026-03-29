#include "matrice.h"

etat_processus_t** allocMat(int nbLig, int nbCol){
	etat_processus_t** mat;
	mat = (TElement**)malloc(nbLig*sizeof(TElement*));
	
	if(mat == NULL)
		perror("Probleme d'allocation");
		
	for(int i=0; i<nbLig; i++)
		mat[i] = allocTab(nbCol);
		//allocTabP(nbCol, &mat[i]);
		
	return mat;
}

etat_processus_t** libMat(int nbLig, etat_processus_t** mat){
	for(int i=0; i<nbLig; i++){
		mat[i] = libTab(mat[i]);
	}
	
	free(mat);
	mat = NULL;
	return mat;
}

void initMat(int nbCol, int nbLig, etat_processus_t** mat) {
	if (mat == NULL)
		return;

	for (int i=0; i<nbLig; i++) {
		for (int j=0; j<nbCol; j++) {
			mat[i][j] = 0;
		}
	}
}