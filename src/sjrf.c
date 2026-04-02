/**
 * @file sjrf.c
 * @brief Logique de l'algorithme d'ordonnancement Shortest Job Remaining First (SJRF).
 * * Stratégie : Préemptif. À chaque tick, le processus ayant le temps de burst
 * CPU restant le plus court est sélectionné.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdio.h>
#include "../include/sjrf.h"
#include "output.h"

static processus_t *selectionner_processus(processus_t *processus, int n, int t);

void run_sjrf(processus_t *processus, int n, resultats_t *resultats, etat_processus_t **gantt) {
	// printf("debut run_sjrf n=%d\n", n);
	int t = 0;
	int termines = 0;
	int temps_non_occupation = 0;

	// printf("apres declarations t=%d termines=%d\n", t, termines);
	while (termines < n) {
		// printf("t=%d etat0=%d etat1=%d\n", t, processus[0].etat, processus[1].etat);
		// printf("t=%d | P1: etat=%d cpu_restant=%d io_restant=%d index=%d | P2: etat=%d cpu_restant=%d io_restant=%d index=%d\n",
		// t,
		// processus[0].etat, processus[0].temps_cpu_restant, processus[0].temps_io_restant, processus[0].index_burst_courant,
		// processus[1].etat, processus[1].temps_cpu_restant, processus[1].temps_io_restant, processus[1].index_burst_courant);
		// break;
		/*
		 * Cette boucle permet de repasser les etats des processus en éxecution en prêt à chaque tour.
		 */
		for (int m = 0; m < n; m++) {
			if (processus[m].etat == ETAT_EN_EXECUTION)
				processus[m].etat = ETAT_PRET;
		}
		// printf("Apres mis a jour des etats a t=%d", t);
		/*
		 * Cela permet à chaque t temps d'arrivée de mettre l'état des processus en prêt, qui permettra par la suite de
		 * sélectionner le plus court temps de processus restant.
		 */
		for (int i = 0; i < n; i++) {
			// printf("processus[%d] etat=%d temps_arrivee=%d\n", i, processus[i].etat, processus[i].temps_arrivee);
			if (processus[i].temps_arrivee <= t && processus[i].etat == ETAT_NOUVEAU)
				processus[i].etat = ETAT_PRET;
		}

		/*
		 * Permet de décrémenter les E/S (I/O) en cours.
		 */
		for (int j = 0; j < n; j++) {
			if (processus[j].etat == ETAT_EN_ATTENTE) {
				processus[j].temps_io_restant--;
				if (processus[j].temps_io_restant == 0) {
					gantt[j][t] = ETAT_EN_ATTENTE;
					int index = ++processus[j].index_burst_courant;
					processus[j].temps_cpu_restant = processus[j].bursts[index];
					processus[j].etat = ETAT_NOUVEAU;
				}
			}
		}

		/*
		 * Ici, on sélectionne le plus court temps de cpu restant.
		 * Si on n'a pas de processus, donc on incrémente le temps de non-occupation du cpu.
		 * Sinon, si c'est la première éxécution du processus, on enregistre le temps de reponse et de début d'exécution à t.
		 * On passe l'état du processus en éxécution, et on décrémente le temps de cpu restant du processus.
		 * Si le temps de cpu restant est à 0, on vérifie si c'est le dernier burst l'état du processus passe à terminer,
		 * on enregistre le temps de fin d'exécution et on incrémente les processus términés.
		 * Sinon le prochain burst c'est en E/S (I/O) et l'état, on le met en attente.
		 */
		processus_t *courant = selectionner_processus(processus, n, t);
		if (courant == NULL) {
			temps_non_occupation++;
			remplir_gantt(gantt, processus, n, t);
		}else {
			if (courant->first_run == 0) {
				courant->temps_reponse = t - courant->temps_arrivee;
				courant->temps_debut_execution = t;
				courant->first_run = 1;
			}

			courant->etat = ETAT_EN_EXECUTION;
			courant->temps_cpu_restant--;
			remplir_gantt(gantt, processus, n, t);

			if (courant->temps_cpu_restant == 0) {
				courant->index_burst_courant++;
				if (courant->index_burst_courant == courant->nb_bursts) {
					courant->temps_fin_execution = t+1;
					courant->etat = ETAT_TERMINE;
					termines++;
				}else {
					courant->temps_io_restant = courant->bursts[courant->index_burst_courant];
					courant->etat = ETAT_EN_ATTENTE;
				}
			}
		}

		/*
		 * Les processus prêts qui ne sont pas élus, on augmente leurs temps d'attente.
		 */
		for (int l = 0; l < n; l++) {
			if (processus[l].etat == ETAT_PRET)
				processus[l].temps_attente++;
		}

		t += 1;
	}

	calcul_metrique(processus, n);
	*resultats = calcul_resultats(processus, n, t, temps_non_occupation);
	resultats->t_max = t;
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
		if (processus[i].etat == ETAT_PRET) {
			if (process == NULL || processus[i].temps_cpu_restant < process->temps_cpu_restant ) {
				process = &processus[i];
			}
		}
	}

	return process;
}