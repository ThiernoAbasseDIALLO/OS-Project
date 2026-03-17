/**
 * @file simulateur.c
 * @brief Boucle de simulation générique non-préemptive orientée événements.
 *
 * La simulation fonctionne par sauts de temps (événements) :
 *   - arrivée d'un processus
 *   - fin d'un burst CPU
 *   - fin d'une E/S
 * À chaque libération du CPU, l'ordonnanceur passé en paramètre choisit
 * le prochain processus à exécuter. Cela rend la fonction `simuler`
 * indépendante de l'algorithme d'ordonnancement utilisé.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "simulateur.h"
#include "queue.h"
#include "csv.h"

/* ---------------------------------------
 * Affichage console
 * -------------------------------------- */

/**
 * @brief Affiche les résultats de simulation dans le terminal.
 * @param liste            Tableau des processus après simulation.
 * @param n                Nombre de processus.
 * @param temps_total      Durée totale de la simulation (ms).
 * @param temps_cpu_occupe Temps pendant lequel le CPU était occupé (ms).
 */
void afficher_resultats_console(processus_t *liste, int n,
                                int temps_total, int temps_cpu_occupe)
{
    double s_att = 0, s_rep = 0, s_res = 0;

    printf("\n-------- RESULTATS DE SIMULATION --------\n");
    printf("PID | Arrivee | Attente | Reponse | Restitution\n");

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

    printf("\nMOYENNES : Attente=%.2f  Reponse=%.2f  Restitution=%.2f\n",
           s_att / n, s_rep / n, s_res / n);
    printf("Taux utilisation CPU : %.2f%%\n",
           100.0 * temps_cpu_occupe / temps_total);
    printf("-----------------------------------------\n\n");
}

/* -----------------------------------
 * Calcul du prochain événement
 * ----------------------------------- */

/**
 * @brief Retourne la durée jusqu'au prochain événement (fin CPU ou fin E/S).
 *
 * On prend le minimum entre :
 *   - le temps CPU restant du processus en cours d'exécution
 *   - le temps E/S restant de chaque processus en attente
 *
 * @param liste        Tableau complet des processus.
 * @param n            Nombre total de processus.
 * @param en_execution Processus actuellement sur le CPU (peut être NULL).
 * @return Delta minimal en ms, INT_MAX si aucun événement détecté.
 */
static int determiner_prochain_evenement(processus_t *liste, int n,
                                         processus_t *en_execution)
{
    int delta = INT_MAX;

    if (en_execution && en_execution->temps_cpu_restant > 0)
        delta = en_execution->temps_cpu_restant;

    for (int i = 0; i < n; i++) {
        if (liste[i].etat == ETAT_EN_ATTENTE &&
            liste[i].temps_io_restant > 0 &&
            liste[i].temps_io_restant < delta)
        {
            delta = liste[i].temps_io_restant;
        }
    }

    return delta;
}

/* ---------------------------------------
 * Boucle de simulation générique
 * --------------------------------------- */

/**
 * @brief Simule l'ordonnancement non-préemptif avec l'algorithme fourni.
 *
 * Étapes de la boucle principale (par événement) :
 *  1. Arrivées : passage NOUVEAU → PRET pour les processus dont
 *     `temps_arrivee <= temps_courant`.
 *  2. Retour après E/S : remise en file PRET des processus dont
 *     `temps_io_restant == 0`.
 *  3. Sélection CPU : si libre, appel à `ordo` pour choisir le processus
 *     à exécuter. Mise à jour du temps d'attente et du temps de réponse.
 *  4. Avance du temps : delta = min(fin CPU courant, fin E/S actives).
 *  5. Mise à jour des compteurs CPU et E/S.
 *  6. Fin de burst CPU : terminaison ou passage en E/S.
 *
 * @param liste    Tableau de processus (modifié en place).
 * @param n        Nombre de processus.
 * @param ordo     Fonction de sélection (FIFO, SJF, ...).
 * @param nom_algo Nom de l'algorithme pour nommer le fichier CSV.
 */
void simuler(processus_t *liste, int n,
             ordonnanceur_t ordo, const char *nom_algo)
{
    int temps_courant    = 0;
    int nb_termines      = 0;
    int temps_cpu_occupe = 0;

    processus_t *en_execution = NULL;
    file_t      *file_prets   = creer_file(n);

    while (nb_termines < n) {

        /* --- 1. Arrivées --- */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_NOUVEAU &&
                liste[i].temps_arrivee <= temps_courant)
            {
                liste[i].etat = ETAT_PRET;
                liste[i].dernier_entree_pret = temps_courant;
                enfiler(file_prets, &liste[i]);
            }
        }

        /* --- 2. Fin des E/S (parallélisées) --- */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_EN_ATTENTE &&
                liste[i].temps_io_restant == 0)
            {
                liste[i].index_burst_courant++;

                if (liste[i].index_burst_courant < liste[i].nb_bursts) {
                    liste[i].temps_cpu_restant =
                        liste[i].bursts[liste[i].index_burst_courant];
                    liste[i].etat = ETAT_PRET;
                    liste[i].dernier_entree_pret = temps_courant;
                    enfiler(file_prets, &liste[i]);
                }
                /* Si plus de bursts : le processus sera terminé (géré en étape 6) */
            }
        }

        /* --- 3. Sélection CPU --- */
        if (en_execution == NULL && file_prets->taille > 0) {
            processus_t *choisi = ordo(file_prets->elements, file_prets->taille);

            /* Localiser l'indice du processus choisi */
            int idx = 0;
            while (idx < file_prets->taille && file_prets->elements[idx] != choisi)
                idx++;

            en_execution = defiler(file_prets, idx);
            en_execution->etat = ETAT_EN_EXECUTION;

            /* Cumul du temps d'attente dans la file */
            en_execution->temps_attente +=
                temps_courant - en_execution->dernier_entree_pret;

            /* Temps de réponse et premier instant d'exécution */
            if (en_execution->temps_debut_execution == -1) {
                en_execution->temps_debut_execution = temps_courant;
                en_execution->temps_reponse =
                    temps_courant - en_execution->temps_arrivee;
            }
        }

        /* --- 4. Avance du temps --- */
        int delta = determiner_prochain_evenement(liste, n, en_execution);
        if (delta == INT_MAX) break; /* Blocage (ne devrait pas se produire) */

        temps_courant += delta;

        /* --- 5. Mise à jour des compteurs --- */
        if (en_execution) {
            en_execution->temps_cpu_restant -= delta;
            temps_cpu_occupe += delta;
        }

        /* Décompte des E/S parallèles */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_EN_ATTENTE)
                liste[i].temps_io_restant -= delta;
        }

        /* --- 6. Fin de burst CPU --- */
        if (en_execution && en_execution->temps_cpu_restant == 0) {
            en_execution->index_burst_courant++;

            if (en_execution->index_burst_courant >= en_execution->nb_bursts) {
                /* Processus entièrement terminé */
                en_execution->etat               = ETAT_TERMINE;
                en_execution->temps_fin_execution = temps_courant;
                en_execution->temps_restitution   =
                    temps_courant - en_execution->temps_arrivee;
                nb_termines++;
            } else {
                /* Passage en E/S */
                en_execution->temps_io_restant =
                    en_execution->bursts[en_execution->index_burst_courant];
                en_execution->etat = ETAT_EN_ATTENTE;
            }

            en_execution = NULL; /* CPU libéré */
        }
    }

    /* Export CSV */
    char nom_fichier[64];
    snprintf(nom_fichier, sizeof(nom_fichier), "resultats_%s.csv", nom_algo);
    exporter_csv(nom_fichier, liste, n, temps_courant, temps_cpu_occupe);

    /* Affichage terminal */
    afficher_resultats_console(liste, n, temps_courant, temps_cpu_occupe);

    liberer_file(file_prets);
}
