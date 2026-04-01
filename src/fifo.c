/**
 * @file fifo.c
 * @brief Logique de l'algorithme d'ordonnancement First-Come, First-Served (FCFS).
 *
 * Stratégie : le processus qui arrive le premier est le premier servi.
 * Non-préemptif. Les E/S sont parallélisées.
 *
 * @sa queue.h
 * Utilise la File de queue.h (initF, enfiler, defiler, sommetF, estVideF).
 * La variable courant est persistante : si le processus n'a pas fini
 * son burst, il garde le CPU au tick suivant sans repasser par la file.
 * Pas d'affichage ni d'export CSV — géré dans main.c.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdlib.h>
#include "../include/fifo.h"
#include "../include/process.h"
#include "queue.h"
#include "../include/output.h"


void run_fifo(processus_t *processus, int n, resultats_t *resultats, etat_processus_t** gantt)
{
    int t                    = 0;
    int termines             = 0;
    int temps_non_occupation = 0;
    processus_t *courant     = NULL;

    File f;
    initF(&f);

    while (termines < n) {
        // printf("t=%d | P1: etat=%d cpu_restant=%d io_restant=%d index=%d | P2: etat=%d cpu_restant=%d io_restant=%d index=%d\n",
        // t,
        // processus[0].etat, processus[0].temps_cpu_restant, processus[0].temps_io_restant, processus[0].index_burst_courant,
        // processus[1].etat, processus[1].temps_cpu_restant, processus[1].temps_io_restant, processus[1].index_burst_courant);
        /* ── Étape 1 : nouvelles arrivées → enfiler ── */
        /*
         * t croît de 1 en 1 : les processus sont enfilés dans l'ordre
         * croissant de temps_arrivee. La tête de file est donc toujours
         * le premier arrivé — sommetF() suffit pour la sélection FIFO.
         */
        for (int i = 0; i < n; i++) {
            if (processus[i].temps_arrivee <= t && processus[i].etat == ETAT_NOUVEAU) {
                processus[i].etat = ETAT_PRET;
                enfiler(&f, &processus[i]);
            }
        }

        /* ── Étape 2 : décrémenter les E/S en cours ── */
        for (int j = 0; j < n; j++) {
            if (processus[j].etat == ETAT_EN_ATTENTE) {
                processus[j].temps_io_restant--;
                if (processus[j].temps_io_restant == 0) {
                    gantt[j][t] = ETAT_EN_ATTENTE;
                    int index = ++processus[j].index_burst_courant;
                    if (index < processus[j].nb_bursts) {
                        /* Burst suivant = CPU → retour en queue de file */
                        processus[j].temps_cpu_restant =
                            processus[j].bursts[index];
                        processus[j].etat = ETAT_NOUVEAU;
                        // enfiler(&f, &processus[j]);
                    } else {
                        /* Dernier burst était une E/S → terminé */
                        processus[j].temps_fin_execution = t+1;
                        processus[j].etat = ETAT_TERMINE;
                        termines++;
                    }
                }
            }
        }

        /* ── Étape 3 : si CPU libre, prendre la tête de file ── */
        /*
         * courant est persistant : tant que le burst n'est pas terminé,
         * le même processus garde le CPU tick après tick sans repasser
         * par la file. On ne prend un nouveau processus que si courant
         * est NULL (CPU libéré à l'étape 4 ou jamais utilisé).
         */
        if (courant == NULL) {
            courant = sommetF(f);
            if (courant != NULL)
                defiler(&f);
        }

        /* ── Étape 4 : exécuter le processus sur le CPU ── */
        if (courant == NULL) {
            /* CPU idle : aucun processus prêt */
            temps_non_occupation++;
            remplir_gantt(gantt, processus, n, t);
        } else {
            /* Premier accès CPU → enregistrer temps de réponse */
            if (courant->first_run == 0) {
                courant->temps_reponse         = t - courant->temps_arrivee;
                courant->temps_debut_execution = t;
                courant->first_run             = 1;
            }

            courant->etat = ETAT_EN_EXECUTION;
            courant->temps_cpu_restant--;
            remplir_gantt(gantt, processus, n, t);

            if (courant->temps_cpu_restant == 0) {
                courant->index_burst_courant++;

                if (courant->index_burst_courant == courant->nb_bursts) {
                    /* Plus de burst → processus terminé */
                    courant->temps_fin_execution = t + 1;
                    courant->etat = ETAT_TERMINE;
                    termines++;
                } else {
                    /* Burst suivant = E/S */
                    courant->temps_io_restant =
                        courant->bursts[courant->index_burst_courant];
                    courant->etat = ETAT_EN_ATTENTE;
                }
                /* CPU libéré → prochain tick prendra la tête de file */
                courant = NULL;
            }
        }

        /* ── Étape 5 : accumuler l'attente des processus PRET ── */
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
