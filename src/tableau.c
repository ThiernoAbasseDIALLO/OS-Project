/**
* @file tableau.c
 * @brief Fonctions utilitaires pour la gestion de tableaux dynamiques d'états.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include "tableau.h"

typedef int TElement;

/**
 * @details Utilise malloc pour réserver la mémoire. Affiche une erreur avec perror()
 * si l'allocation échoue.
 */
etat_processus_t* allocTab(int nMax){
	etat_processus_t* tab;
	tab = (etat_processus_t*)malloc(nMax*sizeof(etat_processus_t));
	
	if(tab==NULL)
		perror("Probleme d'allocation");
		
	return tab;
}

/**
 * @details Cette version modifie directement le pointeur fourni par l'appelant.
 */
void allocTabP(int nMax, etat_processus_t **tab){
	*tab = (etat_processus_t*)malloc(nMax*sizeof(etat_processus_t));
	
	if(*tab == NULL)
		perror("Probleme d'allocation");
}

/**
 * @details Libère la mémoire. Notez que l'appelant doit récupérer la valeur
 * de retour (NULL) pour sécuriser son pointeur.
 */
etat_processus_t* libTab(etat_processus_t *tab){
	free(tab);
	tab = NULL;
	return tab;
}

/**
 * @details Cette version est plus sécurisée, car elle met automatiquement
 * le pointeur de l'appelant à NULL après le free().
 */
void libTabP(etat_processus_t **tab){
	free(*tab);
	*tab = NULL;
}