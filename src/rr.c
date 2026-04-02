/**
 * @file rr.c
 * @brief Logique de l'algorithme d'ordonnancement Round Robin (RR).
 *
 * Stratégie : chaque processus reçoit le CPU pour un quantum fixe.
 * Si son burst n'est pas terminé à l'issue du quantum, il est
 * interrompu et replacé EN FIN de file des prêts (préemptif).
 * Les E/S sont parallélisées.
 *
 * Utilise la File de queue.h pour gérer la file des prêts.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdlib.h>
#include "rr.h"

#include "output.h"
#include "process.h"
#include "queue.h"

void run_rr(processus_t *processus, int n, int quantum, resultats_t *resultats, etat_processus_t **gantt){
    int temps                    = 0;
    int nb_termines              = 0;
    int temps_cpu_inactif        = 0;
    int quantum_restant          = 0;

    processus_t *processus_en_execution         = NULL;
    processus_t *processus_ayant_tourne_ce_tick = NULL;
    processus_t *processus_preempte = NULL;

    File file_prets;
    initF(&file_prets);

    while (nb_termines < n) {
        /* Étape 1 : arrivées */
        for (int i = 0; i < n; i++) {
            if (processus[i].temps_arrivee == temps) {
                processus[i].etat = ETAT_PRET;
                processus[i].dernier_entree_pret = temps - 1;
                enfiler(&file_prets, &processus[i]);
            }
        }

        /* Si un processus a été préempté au tick d'avant, c'est ici qu'on le remet
            en file, APRES les arrivées ou selon une logique de priorité précise. */
        if (processus_preempte != NULL) {
            enfiler(&file_prets, processus_preempte);
            processus_preempte = NULL;
        }

        /* Étape 2 : avancement des E/S */
        for (int j = 0; j < n; j++) {
            if (processus[j].etat == ETAT_EN_ATTENTE) {
                processus[j].temps_io_restant--;
                if (processus[j].temps_io_restant == 0) {
                    gantt[j][temps] = ETAT_EN_ATTENTE;      /* dernier tick E/S */
                    int index = ++processus[j].index_burst_courant;
                    if (index < processus[j].nb_bursts) {
                        processus[j].temps_cpu_restant =
                            processus[j].bursts[index];
                        processus[j].etat = ETAT_PRET;
                        processus[j].dernier_entree_pret = temps;
                        enfiler(&file_prets, &processus[j]);   /* queue.c */
                    } else {
                        processus[j].temps_fin_execution = temps + 1;
                        processus[j].etat = ETAT_TERMINE;
                        nb_termines++;
                    }
                }
            }
        }

        /* Étape 3 : sélection CPU */
        if (processus_en_execution == NULL && !estVideF(file_prets)) {
            /*
             * On cherche le premier candidat dont dernier_entree_pret < temps.
             * Problème : queue.c n'a pas de "peek sans dépiler puis enfiler à nouveau"
             * natif. On dépile, teste, et si le candidat n'est pas éligible
             * on le ré-enfile immédiatement (même comportement qu'avant).
             */
            int essais = 0;
            /* Compter la taille : on parcourt au plus n éléments */
            /* (queue.c ne stocke pas la taille — on borne à n) */
            while (essais < n && !estVideF(file_prets)) {
                processus_t *candidat = sommetF(file_prets);
                defiler(&file_prets);
                if (candidat == NULL) break;

                if (candidat->dernier_entree_pret < temps) {
                    processus_en_execution = candidat;
                    break;
                }
                enfiler(&file_prets, candidat);   /* pas éligible, remet en queue */
                essais++;
            }

            if (processus_en_execution != NULL) {
                quantum_restant = quantum;
                if (processus_en_execution->first_run == 0) {
                    processus_en_execution->temps_reponse = temps - processus_en_execution->temps_arrivee;
                    processus_en_execution->temps_debut_execution = temps;
                    processus_en_execution->first_run = 1;
                }
            }
        }

        /* Étape 4 : tick CPU */
        if (processus_en_execution == NULL) {
            temps_cpu_inactif++;
            remplir_gantt(gantt, processus, n, temps);
        } else {
            processus_ayant_tourne_ce_tick = processus_en_execution;
            processus_en_execution->etat = ETAT_EN_EXECUTION;
            processus_en_execution->temps_cpu_restant--;
            quantum_restant--;

            remplir_gantt(gantt, processus, n, temps);

            if (processus_en_execution->temps_cpu_restant == 0) {
                processus_en_execution->index_burst_courant++;
                if (processus_en_execution->index_burst_courant ==
                        processus_en_execution->nb_bursts) {
                    processus_en_execution->temps_fin_execution = temps + 1;
                    processus_en_execution->etat = ETAT_TERMINE;
                    nb_termines++;
                } else {
                    processus_en_execution->temps_io_restant =
                        processus_en_execution->bursts[
                            processus_en_execution->index_burst_courant];
                    processus_en_execution->etat = ETAT_EN_ATTENTE;
                }
                processus_en_execution = NULL;
                quantum_restant = 0;

            } else if (quantum_restant == 0) {
                /* Préemption : sera remise en file au tick suivant */
                processus_en_execution->etat = ETAT_PRET;
                processus_en_execution->dernier_entree_pret = temps;
                processus_preempte = processus_en_execution;
                processus_en_execution = NULL;
            }
        }

        /* Étape 5 : accumulation temps_attente */
        for (int l = 0; l < n; l++) {
            if (processus[l].etat != ETAT_PRET) continue;
            if (&processus[l] == processus_ayant_tourne_ce_tick) continue;
            if (temps > processus[l].dernier_entree_pret)
                processus[l].temps_attente++;
        }

        processus_ayant_tourne_ce_tick = NULL;
        temps++;
    }

    calcul_metrique(processus, n);
    *resultats = calcul_resultats(processus, n, temps, temps_cpu_inactif);
    resultats->t_max = temps;
}
