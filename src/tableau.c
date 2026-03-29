#include "tableau.h"

typedef int TElement;

etat_processus_t* allocTab(int nMax){
	etat_processus_t* tab;
	tab = (etat_processus_t*)malloc(nMax*sizeof(etat_processus_t));
	
	if(tab==NULL)
		perror("Probleme d'allocation");
		
	return tab;
}

void allocTabP(int nMax, etat_processus_t **tab){
	*tab = (etat_processus_t*)malloc(nMax*sizeof(etat_processus_t));
	
	if(*tab == NULL)
		perror("Probleme d'allocation");
}

etat_processus_t* libTab(etat_processus_t *tab){
	free(tab);
	tab = NULL;
	return tab;
}

void libTabP(etat_processus_t **tab){
	free(*tab);
	*tab = NULL;
}