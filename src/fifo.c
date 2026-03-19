/**
 * @file fifo.c
 * @brief Logique de l'algorithme d'ordonnancement FIFO (FCFS).
 *
 * Stratégie : le processus qui arrive le premier est le premier servi.
 * Non-préemptif. Les E/S sont parallélisées.
 *
 * la sélection se fait directement sur le tableau processus[]
 * en cherchant le plus petit temps_arrivee parmi les prêts.
 * La File de queue.h est utilisée uniquement pour enfiler/défiler
 * les processus prêts — pas pour la sélection.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdlib.h>
#include "fifo.h"
#include "process.h"
#include "queue.h"


/* ----------------------------------
 * Fonction interne : sélection FIFO
 * ---------------------------------- */

/**
 * @brief Sélectionne le processus arrivé le plus tôt parmi les prêts.
 *
 * Parcourt directement le tableau processus[] — identique à le
 * Critère : plus petit temps_arrivee parmi les ETAT_PRET dont
 * temps_arrivee <= t. En cas d'égalité, l'indice le plus bas
 * (ordre du fichier d'entrée) est retenu.
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
        if (processus[i].temps_arrivee <= t &&
            processus[i].etat != ETAT_TERMINE &&
            processus[i].etat != ETAT_EN_ATTENTE)
        {
            if (elu == NULL ||
                processus[i].temps_arrivee < elu->temps_arrivee)
            {
                elu = &processus[i];
            }
        }
    }

    return elu;
}


/* ---------------------------------
 * Fonction exportée : run_fifo
 * ----------------------------------- */

/**
 * @brief Exécute l'algorithme FIFO tick par tick.
 *
 * run_fifo():
 *  1. Repasse EN_EXECUTION → PRET.
 *  2. Active les processus arrivant à t.
 *  3. Décrémente les E/S ; si terminées → PRET ou TERMINE.
 *  4. Sélectionne le processus FIFO (plus petit temps_arrivee).
 *     CPU idle → temps_non_occupation++.
 *     Sinon : enregistre temps_reponse au premier accès,
 *             décrémente temps_cpu_restant,
 *             si burst fini → E/S suivante ou TERMINE.
 *  5. Incrémente temps_attente des processus PRET non élus.
 *  6. t += 1.
 *
 * @param processus Tableau des processus à ordonnancer.
 * @param n         Nombre de processus.
 * @param resultats Pointeur vers la structure de résultats à remplir.
 */
void run_fifo(processus_t *processus, int n, resultats_t *resultats)
{
    int t                    = 0;
    int termines             = 0;
    int temps_non_occupation = 0;

    while (termines < n) {

        /* ── Étape 1 : repasser EN_EXECUTION → PRET ── */
        for (int m = 0; m < n; m++) {
            if (processus[m].etat == ETAT_EN_EXECUTION)
                processus[m].etat = ETAT_PRET;
        }

        /* ── Étape 2 : nouvelles arrivées ── */
        for (int i = 0; i < n; i++) {
            if (processus[i].temps_arrivee == t)
                processus[i].etat = ETAT_PRET;
        }

        /* ── Étape 3 : décrémenter les E/S en cours ── */
        for (int j = 0; j < n; j++) {
            if (processus[j].etat == ETAT_EN_ATTENTE) {
                processus[j].temps_io_restant--;
                if (processus[j].temps_io_restant == 0) {
                    int index = ++processus[j].index_burst_courant;
                    if (index < processus[j].nb_bursts) {
                        /* Burst suivant = CPU */
                        processus[j].temps_cpu_restant =
                            processus[j].bursts[index];
                        processus[j].etat = ETAT_PRET;
                    } else {
                        /* Dernier burst était une E/S → terminé */
                        processus[j].temps_fin_execution = t;
                        processus[j].etat = ETAT_TERMINE;
                        termines++;
                    }
                }
            }
        }

        /* ── Étape 4 : sélection FIFO et exécution ── */
        processus_t *courant = selectionner_processus(processus, n, t);

        if (courant == NULL) {
            temps_non_occupation++;
        } else {
            /* Premier accès CPU → enregistrer temps de réponse */
            if (courant->first_run == 0) {
                courant->temps_reponse         = t;
                courant->temps_debut_execution = t;
                courant->first_run             = 1;
            }

            courant->etat = ETAT_EN_EXECUTION;
            courant->temps_cpu_restant--;

            if (courant->temps_cpu_restant == 0) {
                courant->index_burst_courant++;

                if (courant->index_burst_courant == courant->nb_bursts) {
                    /* Plus de burst → terminé */
                    courant->temps_fin_execution = t + 1;
                    courant->etat = ETAT_TERMINE;
                    termines++;
                } else {
                    /* Burst suivant = E/S */
                    courant->temps_io_restant =
                        courant->bursts[courant->index_burst_courant];
                    courant->etat = ETAT_EN_ATTENTE;
                }
            }
        }

        /* ── Étape 5 : accumuler l'attente des processus PRET ── */
        for (int l = 0; l < n; l++) {
            if (processus[l].etat == ETAT_PRET)
                processus[l].temps_attente++;
        }

        t++;
    }

    calcul_metrique(processus, n);
    *resultats = calcul_resultats(processus, n, t, temps_non_occupation);
}
