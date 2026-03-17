/**
 * @file sjrf.c
 * @brief Implémentation de l'algorithme SJRF (Shortest Job Remaining First).
 *
 * Le SJRF est la version PRÉEMPTIVE du SJF :
 *   - À chaque milliseconde, le processus avec le plus petit temps CPU
 *     RESTANT est élu, même si un autre processus est déjà en cours.
 *   - Si un nouveau processus arrive et que son burst est plus court que
 *     le temps restant du processus courant, il prend le CPU immédiatement.
 *   - Les E/S sont parallélisées : pendant qu'un processus attend ses E/S,
 *     le CPU est attribué à un autre processus.
 *
 * La simulation avance unité par unité (1 ms par tour), ce qui permet la
 * préemption à chaque instant.
 *
 * Indicateurs calculés :
 *   - temps_reponse     : premier instant d'accès CPU - temps_arrivee
 *   - temps_attente     : cumul des ms passées en file PRET sans le CPU
 *   - temps_restitution : temps_fin - temps_arrivee (turnaround)
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include "sjrf.h"
#include "csv.h"

/* ----------------------------------------------------------
 * Sélection du processus (fonction statique, non exportée)
 * ---------------------------------------------------------- */

/**
 * @brief Retourne le processus PRET avec le plus court temps CPU restant.
 *
 * En cas d'égalité, le premier trouvé est gardé (tie-breaking sur l'indice).
 *
 * @param processus Tableau complet des processus.
 * @param n         Nombre de processus.
 * @param t         Temps courant (pour vérifier les arrivées).
 * @return Pointeur vers le processus élu, NULL si aucun processus prêt.
 */
static processus_t *selectionner_processus(processus_t *processus, int n, int t)
{
    processus_t *elu = NULL;

    for (int i = 0; i < n; i++) {
        if (processus[i].temps_arrivee <= t &&
            processus[i].etat == ETAT_PRET)
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

/* ------------------------------------------------------
 * Affichage et export
 * ------------------------------------------------------- */

/**
 * @brief Affiche les résultats dans le terminal et exporte le CSV.
 */
static void afficher_et_exporter_sjrf(processus_t *liste, int n,
                                       int temps_total, int temps_cpu_occupe)
{
    double s_att = 0, s_rep = 0, s_res = 0;

    printf("\n-------- RESULTATS SJRF (preemptif) -------\n");
    printf("PID | Arrivee | Attente | Reponse | Restitution\n");
    printf("----+---------+---------+---------+------------\n");

    for (int i = 0; i < n; i++) {
        printf("%3d | %7d | %7d | %7d | %11d\n",
               liste[i].pid,
               liste[i].temps_arrivee,
               liste[i].temps_attente,
               liste[i].temps_reponse,
               liste[i].temps_restitution);
        s_att += liste[i].temps_attente;
        s_rep += liste[i].temps_reponse;
        s_res += liste[i].temps_restitution;
    }

    printf("----+---------+---------+---------+------------\n");
    printf("MOY | %7s | %7.2f | %7.2f | %11.2f\n",
           "-", s_att / n, s_rep / n, s_res / n);
    printf("Taux utilisation CPU : %.2f%%\n",
           temps_total > 0 ? 100.0 * temps_cpu_occupe / temps_total : 0.0);
    printf("------------------------------------------\n\n");

    exporter_csv("resultats_sjrf.csv", liste, n, temps_total, temps_cpu_occupe);
    printf("Resultats exportes dans resultats_sjrf.csv\n");
}

/* ------------------------------------------------------
 * Boucle de simulation SJRF
 * ------------------------------------------------------ */

/**
 * @brief Simule l'ordonnancement SJRF (preemptif, pas de temps = 1 ms).
 *
 * Structure de la boucle (1 iteration = 1 ms) :
 *
 *  1. Remettre a PRET les processus EN_EXECUTION (preemption possible
 *     a chaque ms : on remet tout le monde en competition).
 *  2. Faire passer en PRET les processus qui arrivent a ce temps t.
 *  3. Decrementer les E/S actives ; si une E/S se termine, remettre
 *     le processus en PRET avec son prochain burst CPU.
 *  4. Selectionner le processus PRET au burst le plus court.
 *     - Si aucun n'est pret : CPU idle.
 *     - Sinon : enregistrer temps_reponse (1ere fois), passer en
 *       ETAT_EN_EXECUTION, decrementer temps_cpu_restant.
 *       Si le burst CPU atteint 0 :
 *         * plus de bursts -> TERMINE, enregistrer metriques
 *         * sinon -> passer en ETAT_EN_ATTENTE avec le burst E/S suivant
 *  5. Incrementer temps_attente des processus restes PRET.
 *  6. Avancer t de 1 ms.
 *
 * @param processus Tableau des processus (modifie en place).
 * @param n         Nombre de processus.
 */
void run_sjrf(processus_t *processus, int n)
{
    int t                = 0;
    int termines         = 0;
    int temps_cpu_occupe = 0;

    while (termines < n) {

        /* --- Etape 1 : preemption — remettre EN_EXECUTION -> PRET --- */
        for (int i = 0; i < n; i++) {
            if (processus[i].etat == ETAT_EN_EXECUTION)
                processus[i].etat = ETAT_PRET;
        }

        /* --- Etape 2 : arrivees --- */
        for (int i = 0; i < n; i++) {
            if (processus[i].etat == ETAT_NOUVEAU &&
                processus[i].temps_arrivee <= t)
            {
                processus[i].etat = ETAT_PRET;
                processus[i].dernier_entree_pret = t;
            }
        }

        /* --- Etape 3 : decompte et fin des E/S --- */
        for (int i = 0; i < n; i++) {
            if (processus[i].etat == ETAT_EN_ATTENTE) {
                processus[i].temps_io_restant--;

                if (processus[i].temps_io_restant == 0) {
                    processus[i].index_burst_courant++;

                    if (processus[i].index_burst_courant < processus[i].nb_bursts) {
                        processus[i].temps_cpu_restant =
                            processus[i].bursts[processus[i].index_burst_courant];
                        processus[i].etat = ETAT_PRET;
                        processus[i].dernier_entree_pret = t;
                    } else {
                        /* Dernier burst etait une E/S : processus termine */
                        processus[i].etat               = ETAT_TERMINE;
                        processus[i].temps_fin_execution = t;
                        processus[i].temps_restitution   =
                            t - processus[i].temps_arrivee;
                        termines++;
                    }
                }
            }
        }

        /* --- Etape 4 : selection et execution (1 ms) --- */
        processus_t *courant = selectionner_processus(processus, n, t);

        if (courant != NULL) {
            /* Premier acces CPU -> temps de reponse */
            if (courant->first_run == 0) {
                courant->temps_debut_execution = t;
                courant->temps_reponse         = t - courant->temps_arrivee;
                courant->first_run             = 1;
            }

            courant->etat = ETAT_EN_EXECUTION;
            courant->temps_cpu_restant--;
            temps_cpu_occupe++;

            /* Fin du burst CPU courant */
            if (courant->temps_cpu_restant == 0) {
                courant->index_burst_courant++;

                if (courant->index_burst_courant >= courant->nb_bursts) {
                    /* Processus entierement termine */
                    courant->temps_fin_execution = t + 1;
                    courant->temps_restitution   = (t + 1) - courant->temps_arrivee;
                    courant->etat                = ETAT_TERMINE;
                    termines++;
                } else {
                    /* Passage en E/S */
                    courant->temps_io_restant =
                        courant->bursts[courant->index_burst_courant];
                    courant->etat = ETAT_EN_ATTENTE;
                }
            }
        }

        /* --- Etape 5 : temps d'attente des processus prets non elus --- */
        for (int i = 0; i < n; i++) {
            if (processus[i].etat == ETAT_PRET)
                processus[i].temps_attente++;
        }

        t++;
    }

    afficher_et_exporter_sjrf(processus, n, t, temps_cpu_occupe);
}
