#include <stdio.h>
#include "../include/sjrf.h"

void run_sjrf(processus_t *processus, int n) {

}

/**
 * @brief Permet de sélectionner le processus avec le plus court temps de cpu restant pour pouvoir choisir un processus
 * @param processus Tableau de tous les processus
 * @param n Nombre d'éléments dans le tableau
 * @param t Temps CPU courant
 * @return Un processus avec le plus court temps de cpu restant
 */
static processus_t *selectionner_processus(processus_t *processus, int n, int t) {
	processus_t *process = NULL;
    for (int i=0; i<n; i++) {
		if (processus[i].temps_arrivee<=t && processus[i].etat != ETAT_TERMINE &&  processus[i].etat != ETAT_EN_ATTENTE) {
			if (process == NULL || processus[i].temps_cpu_restant < process->temps_cpu_restant ) {
				process = &processus[i];
			}
		}
	}

	return process;
}