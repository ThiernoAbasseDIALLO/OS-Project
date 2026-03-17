/**
 * @file sjf.c
 * @brief Implémentation de l'algorithme d'ordonnancement SJF non-préemptif
 *        (Shortest Job First).
 *
 * À chaque fois que le CPU devient libre, on parcourt la file des prêts
 * et on sélectionne le processus dont le burst CPU courant est le plus court.
 * L'algorithme est non-préemptif : une fois un processus lancé, il occupe
 * le CPU jusqu'à la fin de son burst, même si un processus plus court
 * entre dans la file des prêts entre-temps.
 *
 * Les E/S sont parallélisées : pendant qu'un processus fait des E/S,
 * le CPU peut être attribué à un autre processus prêt.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "sjf.h"
#include "process.h"

/* ---------------------------------
 * Structures internes 
 * ---------------------------------- */

/**
 * @brief File d'attente des processus prêts (tableau dynamique).
 */
typedef struct {
    processus_t **elements; /**< Tableau de pointeurs vers les processus prêts */
    int taille;             /**< Nombre de processus actuellement dans la file */
    int capacite;           /**< Capacité allouée */
} file_prets_t;

/* -------------------------------------------
 * Fonctions internes 
 * ----------------------------------------- */

/**
 * @brief Crée et initialise une file de prêts vide.
 */
static file_prets_t *creer_file(int capacite)
{
    file_prets_t *f = malloc(sizeof(file_prets_t));
    if (!f) { perror("malloc file"); exit(1); }
    f->elements = malloc(sizeof(processus_t *) * capacite);
    if (!f->elements) { perror("malloc elements"); exit(1); }
    f->taille   = 0;
    f->capacite = capacite;
    return f;
}

/**
 * @brief Libère la mémoire d'une file de prêts.
 */
static void liberer_file(file_prets_t *f)
{
    free(f->elements);
    free(f);
}

/**
 * @brief Ajoute un processus en fin de file.
 */
static void enfiler(file_prets_t *f, processus_t *p)
{
    if (f->taille >= f->capacite) {
        fprintf(stderr, "File pleine !\n");
        return;
    }
    f->elements[f->taille++] = p;
}

/**
 * @brief Retire le processus à l'indice donné et décale les suivants.
 *
 * Nécessaire pour SJF car le processus choisi n'est pas forcément en tête.
 *
 * @param f      Pointeur vers la file.
 * @param indice Position du processus à retirer.
 * @return Pointeur vers le processus retiré.
 */
static processus_t *defiler_indice(file_prets_t *f, int indice)
{
    if (indice < 0 || indice >= f->taille) return NULL;
    processus_t *p = f->elements[indice];
    for (int i = indice; i < f->taille - 1; i++)
        f->elements[i] = f->elements[i + 1];
    f->taille--;
    return p;
}

/**
 * @brief Calcule le delta de temps jusqu'au prochain événement.
 *
 * Retourne le minimum entre la durée restante du burst CPU en cours
 * et la durée restante des E/S parallèles actives.
 */
static int prochain_evenement(processus_t *liste, int n,
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

/**
 * @brief Affiche les résultats dans le terminal et les exporte en CSV.
 */
static void afficher_et_exporter(processus_t *liste, int n,
                                 int temps_total, int temps_cpu_occupe)
{
    double s_att = 0, s_rep = 0, s_res = 0;

    printf("\n------- RESULTATS SJF (non-preemptif) -------\n");
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
           100.0 * temps_cpu_occupe / temps_total);
    printf("----------------------------------------------\n\n");

    /* Export CSV */
    FILE *f = fopen("resultats_sjf.csv", "w");
    if (!f) { perror("fopen CSV"); return; }

    fprintf(f, "PID;Temps_arrivee;Temps_attente;Temps_reponse;Temps_restitution\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d;%d;%d;%d;%d\n",
                liste[i].pid,
                liste[i].temps_arrivee,
                liste[i].temps_attente,
                liste[i].temps_reponse,
                liste[i].temps_restitution);
    }
    fprintf(f, "\n");
    fprintf(f, "MOYENNE;;%.2f;%.2f;%.2f\n", s_att/n, s_rep/n, s_res/n);
    fprintf(f, "UTILISATION_CPU;;;;%.2f%%\n",
            100.0 * temps_cpu_occupe / temps_total);
    fclose(f);

    printf("Resultats exportes dans resultats_sjf.csv\n");
}

/* ------------------------------
 * Fonctions exportées
 * ------------------------------ */

/**
 * @brief Sélectionne le processus au burst CPU le plus court (SJF).
 *
 * Parcourt l'intégralité de la file des prêts pour trouver le minimum
 * de `temps_cpu_restant`. En cas d'égalité, l'indice le plus petit
 * (= arrivé en premier dans la file, tie-breaking FIFO) est retenu.
 *
 * Complexité : O(taille) à chaque sélection.
 *
 * @param file_prets Tableau de pointeurs vers les processus prêts.
 * @param taille     Nombre de processus dans la file.
 * @return Pointeur vers le processus sélectionné, NULL si la file est vide.
 */
processus_t *ordonnanceur_sjf(processus_t **file_prets, int taille)
{
    if (taille == 0) return NULL;

    int indice_min = 0;

    for (int i = 1; i < taille; i++) {
        /*
         * On prend le burst le plus court.
         * En cas d'égalité (strictement <), on garde le premier trouvé
         * (indice plus petit = dans la file depuis plus longtemps → FIFO).
         */
        if (file_prets[i]->temps_cpu_restant <
            file_prets[indice_min]->temps_cpu_restant)
        {
            indice_min = i;
        }
    }

    return file_prets[indice_min];
}

/**
 * @brief Simule l'ordonnancement SJF non-préemptif sur un ensemble de processus.
 *
 * La boucle est orientée événements :
 *  1. Arrivées : passage NOUVEAU → PRET pour les processus dont
 *     `temps_arrivee <= temps_courant`.
 *  2. Fin d'E/S : remise en file des processus dont `temps_io_restant == 0`.
 *  3. Sélection CPU : si le CPU est libre, appel à `ordonnanceur_sjf` pour
 *     choisir le processus au burst le plus court parmi les prêts.
 *  4. Avance du temps jusqu'au prochain événement (fin CPU ou fin E/S).
 *  5. Mise à jour des compteurs et des états.
 *
 * Différence avec FIFO : seule l'étape 3 change (on cherche le min
 * au lieu de prendre systématiquement la tête de file).
 *
 * @param liste Tableau de processus (modifié en place).
 * @param n     Nombre de processus.
 */
void simuler_sjf(processus_t *liste, int n)
{
    int temps_courant    = 0;
    int nb_termines      = 0;
    int temps_cpu_occupe = 0;

    processus_t  *en_execution = NULL;
    file_prets_t *file         = creer_file(n);

    while (nb_termines < n) {

        /* --- Étape 1 : arrivées --- */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_NOUVEAU &&
                liste[i].temps_arrivee <= temps_courant)
            {
                liste[i].etat = ETAT_PRET;
                liste[i].dernier_entree_pret = temps_courant;
                enfiler(file, &liste[i]);
            }
        }

        /* --- Étape 2 : retour en file après E/S terminées --- */
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
                    enfiler(file, &liste[i]);
                }
            }
        }

        /* --- Étape 3 : sélection CPU (SJF) --- */
        if (en_execution == NULL && file->taille > 0) {
            /* SJF : on cherche le processus au burst CPU le plus court */
            processus_t *choisi = ordonnanceur_sjf(file->elements, file->taille);

            /* Localiser l'indice du processus choisi dans la file */
            int idx = 0;
            while (idx < file->taille && file->elements[idx] != choisi)
                idx++;

            en_execution = defiler_indice(file, idx);
            en_execution->etat = ETAT_EN_EXECUTION;

            /* Cumul du temps d'attente dans la file */
            en_execution->temps_attente +=
                temps_courant - en_execution->dernier_entree_pret;

            /* Temps de réponse : premier accès CPU seulement */
            if (en_execution->temps_debut_execution == -1) {
                en_execution->temps_debut_execution = temps_courant;
                en_execution->temps_reponse =
                    temps_courant - en_execution->temps_arrivee;
            }
        }

        /* --- Étape 4 : avance du temps --- */
        int delta = prochain_evenement(liste, n, en_execution);
        if (delta == INT_MAX) break;

        temps_courant += delta;

        if (en_execution) {
            en_execution->temps_cpu_restant -= delta;
            temps_cpu_occupe += delta;
        }

        /* E/S parallèles */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_EN_ATTENTE)
                liste[i].temps_io_restant -= delta;
        }

        /* --- Étape 5 : fin du burst CPU --- */
        if (en_execution && en_execution->temps_cpu_restant == 0) {
            en_execution->index_burst_courant++;

            if (en_execution->index_burst_courant >= en_execution->nb_bursts) {
                /* Processus terminé */
                en_execution->etat = ETAT_TERMINE;
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

            en_execution = NULL;
        }
    }

    afficher_et_exporter(liste, n, temps_courant, temps_cpu_occupe);
    liberer_file(file);
}
