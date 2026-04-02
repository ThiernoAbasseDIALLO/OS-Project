/**
 * @file sjf.c
 * @brief Logique de l'algorithme d'ordonnancement SJF non-préemptif.
 *
 * Stratégie : parmi tous les processus prêts, on exécute celui dont
 * le burst CPU courant est le plus court. Non-préemptif.
 * Les E/S sont parallélisées.
 *
 * La variable courant est persistante : une fois sélectionné, le
 * processus garde le CPU jusqu'à la fin de son burst, même si un
 * processus plus court arrive entre-temps.
 * C'est ce qui différencie SJF (non-préemptif) de SJRF (préemptif).
 * Pas d'affichage ni d'export CSV — géré dans main.c.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdlib.h>
#include "../include/sjf.h"
#include "../include/process.h"
#include "../include/output.h"

/**
 * @brief Sélectionne le processus au plus court burst CPU restant.
 *
 * Parcourt directement le tableau processus[].
 * Critère : plus petit temps_cpu_restant parmi les processus dont
 * temps_arrivee <= t, non terminés, non en attente E/S.
 * En cas d'égalité, l'indice le plus bas est retenu.
 *
 * Appelée UNIQUEMENT quand courant == NULL (CPU libre).
 * Jamais appelée en cours de burst → non-préemptif garanti.
 *
 * @param processus Tableau de tous les processus.
 * @param n         Nombre de processus.
 * @param t         Temps courant.
 * @return Pointeur vers le processus sélectionné, NULL si aucun prêt.
 */
static processus_t *selectionner_processus(processus_t *processus, int n, int t)
{
    processus_t *elu = NULL;

    for (int i = 0; i < n; i++) {
        if (processus[i].temps_arrivee <= t && processus[i].etat == ETAT_PRET)
        {
            if (elu == NULL ||
                processus[i].temps_cpu_restant < elu->temps_cpu_restant)
            {
                elu = &processus[i];
            }
        }
    }

    return elu;
}

/**
 * @brief Exécute l'algorithme SJF non-préemptif tick par tick.
 *
 *  1. Nouvelles arrivées → ETAT_PRET si temps_arrivee == t.
 *  2. Décrémenter les E/S ; si terminées → ETAT_PRET ou TERMINE.
 *  3. Si courant == NULL (CPU libre) → selectionner_processus().
 *     La sélection n'a lieu QUE si le CPU est libre.
 *     Un processus en cours de burst ne peut pas être interrompu.
 *  4. Exécuter courant : premier accès → temps_reponse,
 *     décrémenter temps_cpu_restant,
 *     si burst fini → E/S suivante ou TERMINE, courant = NULL.
 *  5. Accumuler temps_attente des processus PRET non élus.
 *  6. t += 1.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param resultats Pointeur vers la structure de résultats à remplir.
 */
void run_sjf(processus_t *processus, int n, resultats_t *resultats, etat_processus_t** gantt){
    int t                    = 0;
    int termines             = 0;
    int temps_non_occupation = 0;
    processus_t *courant     = NULL;  /* Persistant : garde le CPU jusqu'à fin burst */

    while (termines < n) {
        // printf("t=%d | P1: etat=%d cpu_restant=%d io_restant=%d index=%d | P2: etat=%d cpu_restant=%d io_restant=%d index=%d\n",
        // t,
        // processus[0].etat, processus[0].temps_cpu_restant, processus[0].temps_io_restant, processus[0].index_burst_courant,
        // processus[1].etat, processus[1].temps_cpu_restant, processus[1].temps_io_restant, processus[1].index_burst_courant);

        /* ── Étape 1 : nouvelles arrivées ── */
        for (int i = 0; i < n; i++) {
            if (processus[i].temps_arrivee <= t && processus[i].etat == ETAT_NOUVEAU)
                processus[i].etat = ETAT_PRET;
        }

        /* ── Étape 2 : décrémenter les E/S en cours ── */
        for (int j = 0; j < n; j++) {
            if (processus[j].etat == ETAT_EN_ATTENTE) {
                processus[j].temps_io_restant--;
                if (processus[j].temps_io_restant == 0) {
                    gantt[j][t] = ETAT_EN_ATTENTE;
                    int index = ++processus[j].index_burst_courant;
                    if (index < processus[j].nb_bursts) {
                        /* Burst suivant = CPU */
                        processus[j].temps_cpu_restant =
                            processus[j].bursts[index];
                        processus[j].etat = ETAT_NOUVEAU;
                    } else {
                        /* Dernier burst était une E/S → terminé */
                        processus[j].temps_fin_execution = t + 1;
                        processus[j].etat = ETAT_TERMINE;
                        termines++;
                    }
                }
            }
        }

        /* ── Étape 3 : sélection SJF uniquement si CPU libre ── */
        /*
         * On ne sélectionne un nouveau processus QUE
         * si courant == NULL. Si un processus plus court arrive pendant
         * l'exécution de courant, il devra attendre la fin du burst.
         */
        if (courant == NULL)
            courant = selectionner_processus(processus, n, t);

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
                    courant->temps_fin_execution = t+1;
                    courant->etat = ETAT_TERMINE;
                    termines++;
                } else {
                    /* Burst suivant = E/S */
                    courant->temps_io_restant =
                        courant->bursts[courant->index_burst_courant];
                    courant->etat = ETAT_EN_ATTENTE;
                }
                /* CPU libéré → prochain tick cherchera le plus court */
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
