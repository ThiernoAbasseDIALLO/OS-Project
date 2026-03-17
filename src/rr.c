/**
 * @file rr.c
 * @brief Implémentation de l'algorithme d'ordonnancement Round Robin (RR).
 *
 * Principe du Round Robin :
 *   Chaque processus prêt reçoit le CPU pour une tranche de temps fixe
 *   (le quantum). Si son burst CPU n'est pas terminé à l'issue du quantum,
 *   il est interrompu et replacé EN FIN de file des prêts. Le processus
 *   suivant en tête de file est alors élu.
 *
 * Gestion des E/S :
 *   Quand un processus termine son burst CPU avant épuisement du quantum,
 *   s'il lui reste des bursts, il passe en E/S (ETAT_EN_ATTENTE). Les E/S
 *   se déroulent en parallèle. Quand elles se terminent, le processus
 *   rejoint LA FIN de la file des prêts (comme s'il venait d'arriver).
 *
 * Gestion du quantum résiduel :
 *   Le quantum repart à zéro à chaque fois qu'un nouveau processus prend
 *   le CPU (y compris si c'est le même processus qui reprend après un retour
 *   d'E/S — il reçoit un quantum complet).
 *
 * La simulation avance par événements :
 *   Delta = min(quantum_restant, prochain_fin_E/S, prochain_arrivee)
 *   ce qui évite de tourner ms par ms tout en restant exact.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "rr.h"
#include "csv.h"

/* -----------------------------------------------------------------------
 * File circulaire FIFO (tableau dynamique) — interne à rr.c
 * --------------------------------------------------------------------- */

/**
 * @brief File FIFO pour le Round Robin.
 *
 * Les processus sont enfilés en queue et défilés en tête.
 * L'ordre dans la file reflète l'ordre d'arrivée/retour dans la file des
 * prêts, ce qui est fondamental pour la politique RR.
 */
typedef struct {
    processus_t **elements;
    int tete;       /**< Indice du premier élément (à défiler) */
    int queue;      /**< Indice du prochain emplacement libre (à enfiler) */
    int taille;     /**< Nombre d'éléments actuellement dans la file */
    int capacite;   /**< Capacité totale du tableau circulaire */
} file_rr_t;

/**
 * @brief Crée une file circulaire de capacité donnée.
 */
static file_rr_t *creer_file_rr(int capacite)
{
    /* +1 pour distinguer file pleine / file vide dans le tableau circulaire */
    int cap = capacite + 1;
    file_rr_t *f = malloc(sizeof(file_rr_t));
    if (!f) { perror("malloc file_rr"); exit(1); }
    f->elements = malloc(sizeof(processus_t *) * cap);
    if (!f->elements) { perror("malloc elements rr"); exit(1); }
    f->tete     = 0;
    f->queue    = 0;
    f->taille   = 0;
    f->capacite = cap;
    return f;
}

/**
 * @brief Libère la mémoire d'une file RR.
 */
static void liberer_file_rr(file_rr_t *f)
{
    free(f->elements);
    free(f);
}

/**
 * @brief Retourne 1 si la file est vide.
 */
static int file_rr_vide(const file_rr_t *f)
{
    return f->taille == 0;
}

/**
 * @brief Ajoute un processus en queue (fin de file).
 */
static void enfiler_rr(file_rr_t *f, processus_t *p)
{
    if (f->taille >= f->capacite - 1) {
        fprintf(stderr, "enfiler_rr : file pleine\n");
        return;
    }
    f->elements[f->queue] = p;
    f->queue = (f->queue + 1) % f->capacite;
    f->taille++;
}

/**
 * @brief Retire et retourne le processus en tête de file.
 */
static processus_t *defiler_rr(file_rr_t *f)
{
    if (file_rr_vide(f)) return NULL;
    processus_t *p = f->elements[f->tete];
    f->tete = (f->tete + 1) % f->capacite;
    f->taille--;
    return p;
}

/* -----------------------------------------------------------------------
 * Calcul du prochain événement
 * --------------------------------------------------------------------- */

/**
 * @brief Calcule le delta jusqu'au prochain événement pertinent.
 *
 * On prend le minimum entre :
 *   - le quantum restant (fin de tranche du processus en cours)
 *   - la fin de chaque E/S en cours (parallélisées)
 *   - la prochaine arrivée de processus (pour ne pas rater d'arrivée)
 *
 * @param liste           Tableau complet des processus.
 * @param n               Nombre total de processus.
 * @param en_execution    Processus sur le CPU (peut être NULL).
 * @param quantum_restant Temps restant dans le quantum courant.
 * @param temps_courant   Temps actuel de la simulation.
 * @return Delta minimal en ms.
 */
static int prochain_evenement_rr(processus_t *liste, int n,
                                  processus_t *en_execution,
                                  int quantum_restant,
                                  int temps_courant)
{
    int delta = INT_MAX;

    /* Fin du burst CPU courant (peut être < quantum_restant) */
    if (en_execution && en_execution->temps_cpu_restant > 0) {
        int fin_cpu = en_execution->temps_cpu_restant;
        int fin_quantum = quantum_restant;
        /* On s'arrête au plus tôt : fin CPU ou fin quantum */
        delta = (fin_cpu < fin_quantum) ? fin_cpu : fin_quantum;
    } else if (en_execution == NULL) {
        /* CPU idle : on attend la prochaine fin E/S ou arrivée */
    }

    /* Fin d'une E/S parallèle */
    for (int i = 0; i < n; i++) {
        if (liste[i].etat == ETAT_EN_ATTENTE &&
            liste[i].temps_io_restant > 0 &&
            liste[i].temps_io_restant < delta)
        {
            delta = liste[i].temps_io_restant;
        }
    }

    /* Prochaine arrivée (pour ne pas la rater si le CPU est idle) */
    for (int i = 0; i < n; i++) {
        if (liste[i].etat == ETAT_NOUVEAU) {
            int attente = liste[i].temps_arrivee - temps_courant;
            if (attente > 0 && attente < delta)
                delta = attente;
        }
    }

    return delta;
}

/* ---------------------------
 * Affichage et export
 * ---------------------------- */

/**
 * @brief Affiche les résultats dans le terminal et exporte le CSV.
 */
static void afficher_et_exporter_rr(processus_t *liste, int n,
                                     int temps_total, int temps_cpu_occupe,
                                     int quantum)
{
    double s_att = 0, s_rep = 0, s_res = 0;

    printf("\n------- RESULTATS RR (quantum=%d ms) -------\n", quantum);
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
    printf("-----------------------------------------------------------\n\n");

    /* Nom de fichier incluant le quantum pour pouvoir comparer */
    char nom_csv[64];
    snprintf(nom_csv, sizeof(nom_csv), "resultats_rr_q%d.csv", quantum);
    exporter_csv(nom_csv, liste, n, temps_total, temps_cpu_occupe);
    printf("Resultats exportes dans %s\n", nom_csv);
}

/* ------------------------------------
 * Boucle de simulation Round Robin
 * ------------------------------------ */

/**
 * @brief Simule l'ordonnancement Round Robin (préemptif, par quantum).
 *
 * Boucle orientée événements (pas d'avance ms par ms) :
 *
 *  1. Faire passer en PRET les processus arrivés (NOUVEAU -> PRET).
 *     Les remettre en queue de la file RR.
 *  2. Remettre en file les processus dont les E/S viennent de se terminer.
 *  3. Si le CPU est libre, défiler le prochain processus (tête de file RR)
 *     et lui attribuer un quantum complet.
 *     Enregistrer le temps de réponse (premier accès seulement).
 *  4. Calculer le delta (min quantum_restant, fin E/S, prochaine arrivée).
 *  5. Avancer le temps, décompter CPU, E/S.
 *  6. Gérer la fin de tranche (expiration quantum ou fin de burst CPU) :
 *     - Expiration quantum : remettre le processus EN FIN de file des prêts.
 *     - Fin burst CPU avant quantum : passer en E/S ou TERMINE.
 *  7. Comptabiliser le temps d'attente des processus prêts non élus.
 *
 * @param liste   Tableau des processus (modifié en place).
 * @param n       Nombre de processus.
 * @param quantum Durée du quantum (ms).
 */
void simuler_rr(processus_t *liste, int n, int quantum)
{
    if (quantum <= 0) {
        fprintf(stderr, "simuler_rr : quantum doit etre > 0\n");
        return;
    }

    int temps_courant    = 0;
    int nb_termines      = 0;
    int temps_cpu_occupe = 0;
    int quantum_restant  = 0;  /* Quantum restant pour le processus en cours */

    processus_t *en_execution = NULL;
    file_rr_t   *file         = creer_file_rr(n * 4); /* x4 : un processus peut
                                                          repasser plusieurs fois */

    while (nb_termines < n) {

        /* --- Étape 1 : arrivées --- */
        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_NOUVEAU &&
                liste[i].temps_arrivee <= temps_courant)
            {
                liste[i].etat = ETAT_PRET;
                liste[i].dernier_entree_pret = temps_courant;
                enfiler_rr(file, &liste[i]);
            }
        }

        /* --- Étape 2 : retour en file après fin d'E/S --- */
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
                    enfiler_rr(file, &liste[i]); /* En fin de file RR */
                } else {
                    /* Dernier burst était une E/S */
                    liste[i].etat               = ETAT_TERMINE;
                    liste[i].temps_fin_execution = temps_courant;
                    liste[i].temps_restitution   =
                        temps_courant - liste[i].temps_arrivee;
                    nb_termines++;
                }
            }
        }

        /* --- Étape 3 : attribution du CPU si libre --- */
        if (en_execution == NULL && !file_rr_vide(file)) {
            en_execution = defiler_rr(file);
            en_execution->etat = ETAT_EN_EXECUTION;
            quantum_restant = quantum; /* Nouveau quantum complet */

            /* Temps d'attente passé dans la file */
            en_execution->temps_attente +=
                temps_courant - en_execution->dernier_entree_pret;

            /* Temps de réponse : uniquement lors du tout premier accès CPU */
            if (en_execution->temps_debut_execution == -1) {
                en_execution->temps_debut_execution = temps_courant;
                en_execution->temps_reponse =
                    temps_courant - en_execution->temps_arrivee;
            }
        }

        /* --- Étape 4 : calcul du delta --- */
        int delta = prochain_evenement_rr(liste, n, en_execution,
                                          quantum_restant, temps_courant);
        if (delta == INT_MAX) break; /* Blocage (ne devrait pas arriver) */

        temps_courant += delta;

        /* --- Étape 5 : décompte CPU et E/S --- */
        if (en_execution) {
            en_execution->temps_cpu_restant -= delta;
            quantum_restant                 -= delta;
            temps_cpu_occupe                += delta;
        }

        for (int i = 0; i < n; i++) {
            if (liste[i].etat == ETAT_EN_ATTENTE)
                liste[i].temps_io_restant -= delta;
        }

        /* --- Étape 6 : fin de tranche ou fin de burst CPU --- */
        if (en_execution) {

            if (en_execution->temps_cpu_restant == 0) {
                /*
                 * Le processus a terminé son burst CPU avant (ou en même
                 * temps que) la fin du quantum.
                 */
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

                en_execution = NULL;

            } else if (quantum_restant == 0) {
                /*
                 * Expiration du quantum : le processus est interrompu.
                 * Il rejoint la FIN de la file des prêts.
                 */
                en_execution->etat = ETAT_PRET;
                en_execution->dernier_entree_pret = temps_courant;
                enfiler_rr(file, en_execution); /* En fin de file RR */
                en_execution = NULL;
            }
        }
    }

    afficher_et_exporter_rr(liste, n, temps_courant, temps_cpu_occupe, quantum);
    liberer_file_rr(file);
}
