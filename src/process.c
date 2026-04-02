/**
 * @file process.c
 * @brief Implémentation des outils de gestion des processus et des calculs.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include "process.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX_BURSTS 20

void initialiser_processus(processus_t *p)
{
    /* Reset des compteurs et états par défaut */
    p->index_burst_courant = 0;
    p->temps_cpu_restant = p->bursts[0];    // Le premier burst est par convention CPU
    p->temps_io_restant = 0;
    p->etat = ETAT_NOUVEAU;

    /* Initialisation des métriques à des valeurs neutres */
    p->first_run = 0;
    p->temps_debut_execution = -1;   
    p->temps_fin_execution = 0;
    p->temps_attente = 0;
    p->temps_reponse = -1;           
    p->temps_restitution = 0;
}

processus_t *allocProcessus(int n) {
    processus_t *p = (processus_t *) malloc(n*sizeof(processus_t));

    if (p == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(1);
    }

    return p;
}

float temps_attente_moyenne(processus_t *p, int n) {
    float mean = 0;
    for (int i = 0; i < n; i++) {
        mean += p[i].temps_attente;
    }

    return (n > 0) ? (mean / n) : 0;
}

void calcul_metrique(processus_t *p, int n) {
    /* Calcul du Turnaround Time pour chaque processus */
    for (int i = 0; i < n; i++) {
        p[i].temps_restitution = p[i].temps_fin_execution -
            p[i].temps_arrivee;
    }
}

resultats_t calcul_resultats(processus_t *p, int n, int temps_total, int temps_n_occupation) {
    resultats_t res = init_resultats();

    res.moyenne_attente = temps_attente_moyenne(p, n);

    float mean_restitution = 0;
    float mean_reponse = 0;

    for (int i = 0; i < n; i++) {
        mean_restitution += p[i].temps_restitution;
        mean_reponse += p[i].temps_reponse;
    }

    if (n > 0) {
        res.moyenne_restitution = mean_restitution / n;
        res.moyenne_reponse = mean_reponse / n;
    }

    /* Formule du taux d'occupation : (Temps total - Temps de non-occupation) / Temps total */
    if (temps_total > 0) {
        res.taux_occupation = (float)(temps_total - temps_n_occupation) / temps_total;
    }

    return res;
}

processus_t *lireFichier(char* file, int *n) {
    /* * Lecture du fichier au format :
     * Ligne 1 : Nombre de processus
     * Lignes suivantes : Temps_Arrivée Burst_CPU1 Burst_IO1 Burst_CPU2 ...
     */
    char buffer[256];
    FILE *f = fopen(file, "r");

    if (!f) {
        perror("Erreur d'ouvrir le fichier");
        exit(-1);
    }

    fgets(buffer, sizeof(buffer), f);

    *n = (int)strtol(buffer, NULL, 10);
    processus_t* p = allocProcessus(*n);

    for (int i = 0; i < *n; i++) {
        if (fgets(buffer, sizeof(buffer), f) == NULL) break;

        p[i].bursts = malloc(MAX_BURSTS * sizeof(int));

        char *ptr = buffer;
        char *fin;
        int cpt = 0;

        /* Découpage de la ligne pour extraire les entiers */
        while (*ptr != '\n' && *ptr != '\0') {
            if (cpt == 0) {
                p[i].temps_arrivee = (int)strtol(ptr, &fin, 10);
            }else {
                p[i].bursts[cpt-1] = (int)strtol(ptr, &fin, 10);
            }

            if (fin == ptr) break;

            cpt++;
            ptr = fin;
        }

        p[i].nb_bursts = cpt - 1;
        initialiser_processus(&p[i]);
    }

    fclose(f);
    return p;
}

resultats_t init_resultats() {
    resultats_t resultats;
    resultats.moyenne_attente = 0;
    resultats.moyenne_reponse = 0;
    resultats.moyenne_restitution = 0;
    resultats.taux_occupation = 0;
    resultats.t_max = 0;
    return resultats;
}
