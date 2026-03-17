/**
 * @file fifo.c
 * @brief Implémentation de l'algorithme d'ordonnancement FIFO .
 *
 * Stratégie : le processus qui arrive le premier dans la file des prêts
 * est le premier servi. Non-préemptif : le CPU n'est pas retiré au
 * processus en cours d'exécution, même si un nouveau processus arrive.
 *
 * Les E/S sont parallélisées : pendant qu'un processus fait des E/S,
 * un autre peut utiliser le CPU.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "fifo.h"
#include "process.h"

/* -------------------------------------
 * Structures internes
 * ------------------------------------ */

/**
 * @brief File d'attente des processus prêts (tableau dynamique).
 *
 * On utilise un tableau de pointeurs vers processus_t, avec une taille
 * courante. L'ordre du tableau reflète l'ordre d'arrivée dans la file
 * (FIFO strict).
 */
typedef struct {
    processus_t **elements; /**< Tableau de pointeurs vers les processus prêts */
    int taille;             /**< Nombre de processus actuellement dans la file */
    int capacite;           /**< Capacité allouée du tableau */
} file_prets_t;

/* ----------------------------------------
 * Fonctions internes 
 * ------------------------------------------ */

/**
 * @brief Crée et initialise une file de prêts vide.
 * @param capacite  Capacité maximale (nombre de processus).
 * @return Pointeur vers la file allouée.
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
 * @param f Pointeur vers la file à libérer.
 */
static void liberer_file(file_prets_t *f)
{
    free(f->elements);
    free(f);
}

/**
 * @brief Ajoute un processus en fin de file (enfilage FIFO).
 * @param f Pointeur vers la file.
 * @param p Pointeur vers le processus à ajouter.
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
 * @brief Retire un processus à l'indice donné et décale les suivants.
 * @param f      Pointeur vers la file.
 * @param indice Position du processus à retirer.
 * @return Pointeur vers le processus retiré.
 */
static processus_t *defiler_indice(file_prets_t *f, int indice)
{
    if (indice < 0 || indice >= f->taille) return NULL;
    processus_t *p = f->elements[indice];
    /* Décalage vers la gauche */
    for (int i = indice; i < f->taille - 1; i++)
        f->elements[i] = f->elements[i + 1];
    f->taille--;
    return p;
}

/**
 * @brief Calcule le delta de temps jusqu'au prochain événement.
 *
 * Un événement est soit la fin du burst CPU en cours, soit la fin
 * d'une E/S parallèle. On retourne le minimum de ces durées.
 *
 * @param liste        Tableau complet des processus.
 * @param n            Nombre total de processus.
 * @param en_execution Processus actuellement sur le CPU (peut être NULL).
 * @return Durée en ms jusqu'au prochain événement.
 */
static int prochain_evenement(processus_t *liste, int n,
                              processus_t *en_execution)
{
    int delta = INT_MAX;

    /* Fin du burst CPU courant */
    if (en_execution && en_execution->temps_cpu_restant > 0)
        delta = en_execution->temps_cpu_restant;

    /* Fin d'une E/S en cours (parallélisées) */
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
 * @param liste             Tableau des processus après simulation.
 * @param n                 Nombre de processus.
 * @param temps_total       Durée totale de la simulation (ms).
 * @param temps_cpu_occupe  Durée pendant laquelle le CPU était occupé (ms).
 */
static void afficher_et_exporter(processus_t *liste, int n,
                                 int temps_total, int temps_cpu_occupe)
{
    double s_att = 0, s_rep = 0, s_res = 0;

    printf("\n------- RESULTATS FIFO -------\n");
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
    printf("------------------------------------\n\n");

    /* Export CSV */
    FILE *f = fopen("resultats_fifo.csv", "w");
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

    printf("Resultats exportes dans resultats_fifo.csv\n");
}

/* --------------------------------
 * Fonctions exportées
 * -------------------------------- */

/**
 * @brief Sélectionne le premier processus dans la file des prêts (FIFO).
 *
 * En FIFO, on retourne simplement l'élément en tête de file (index 0),
 * qui est celui qui attend depuis le plus longtemps.
 *
 * @param file_prets Tableau de pointeurs vers les processus prêts.
 * @param taille     Nombre de processus dans la file.
 * @return Pointeur vers le processus sélectionné, NULL si la file est vide.
 */
processus_t *ordonnanceur_fifo(processus_t **file_prets, int taille)
{
    if (taille == 0) return NULL;
    return file_prets[0]; /* Tête de file = premier arrivé */
}

/**
 * @brief Simule l'ordonnancement FIFO sur un ensemble de processus.
 *
 * Boucle principale orientée événements :
 *  1. Faire passer en PRET les processus dont l'heure d'arrivée est atteinte.
 *  2. Remettre en file les processus dont les E/S se terminent.
 *  3. Si le CPU est libre, choisir le prochain processus via FIFO.
 *  4. Avancer le temps jusqu'au prochain événement (fin CPU ou fin E/S).
 *  5. Mettre à jour les compteurs et les états.
 *
 * @param liste Tableau de processus (modifié en place).
 * @param n     Nombre de processus.
 */
void simuler_fifo(processus_t *liste, int n)
{
    int temps_courant    = 0;
    int nb_termines      = 0;
    int temps_cpu_occupe = 0;

    processus_t  *en_execution = NULL;
    file_prets_t *file         = creer_file(n);

    while (nb_termines < n) {

        /* --- Étape 1 : arrivées de nouveaux processus --- */
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
                    /* Burst suivant = CPU */
                    liste[i].temps_cpu_restant =
                        liste[i].bursts[liste[i].index_burst_courant];
                    liste[i].etat = ETAT_PRET;
                    liste[i].dernier_entree_pret = temps_courant;
                    enfiler(file, &liste[i]);
                }
                /* Si plus de bursts, le processus reste TERMINE (géré plus bas) */
            }
        }

        /* --- Étape 3 : sélection CPU (FIFO) --- */
        if (en_execution == NULL && file->taille > 0) {
            /* FIFO : on prend toujours le premier de la file */
            processus_t *choisi = ordonnanceur_fifo(file->elements, file->taille);

            /* Trouver son indice dans le tableau (sera 0 en FIFO) */
            int idx = 0;
            while (idx < file->taille && file->elements[idx] != choisi)
                idx++;

            en_execution = defiler_indice(file, idx);
            en_execution->etat = ETAT_EN_EXECUTION;

            /* Cumul du temps d'attente dans la file */
            en_execution->temps_attente +=
                temps_courant - en_execution->dernier_entree_pret;

            /* Temps de réponse : uniquement lors du premier accès CPU */
            if (en_execution->temps_debut_execution == -1) {
                en_execution->temps_debut_execution = temps_courant;
                en_execution->temps_reponse =
                    temps_courant - en_execution->temps_arrivee;
            }
        }

        /* --- Étape 4 : calcul du delta et avance du temps --- */
        int delta = prochain_evenement(liste, n, en_execution);
        if (delta == INT_MAX) break; /* Blocage (ne devrait pas arriver) */

        temps_courant += delta;

        /* Décompte du burst CPU en cours */
        if (en_execution) {
            en_execution->temps_cpu_restant -= delta;
            temps_cpu_occupe += delta;
        }

        /* Décompte des E/S parallèles */
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
                en_execution->temps_fin_execution  = temps_courant;
                en_execution->temps_restitution    =
                    temps_courant - en_execution->temps_arrivee;
                nb_termines++;
            } else {
                /* Passage en phase E/S */
                en_execution->temps_io_restant =
                    en_execution->bursts[en_execution->index_burst_courant];
                en_execution->etat = ETAT_EN_ATTENTE;
            }

            en_execution = NULL; /* CPU libéré */
        }
    }

    afficher_et_exporter(liste, n, temps_courant, temps_cpu_occupe);
    liberer_file(file);
}
