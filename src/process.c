#include "process.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX_BURSTS 20

/*
 * Initialisation complète d’un processus.
 * Cette fonction doit être appelée après le chargement
 * des données (PID, temps d’arrivée, bursts, nb_bursts).
 */

void initialiser_processus(processus_t *p)
{
    /* Positionnement initial */
    p->index_burst_courant = 0;

    /* Initialement, le premier burst est toujours CPU */
    p->temps_cpu_restant = p->bursts[0];

    /* Aucun I/O actif au départ */
    p->temps_io_restant = 0;

    /* Etat initial */
    p->etat = ETAT_NOUVEAU;

    /* Indicateurs de performance */
    p->first_run = 0;
    p->temps_debut_execution = -1;   
    p->temps_fin_execution = 0;

    p->temps_attente = 0;
    p->temps_reponse = -1;           
    p->temps_restitution = 0;

    p->dernier_entree_pret = 0;
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

    mean /= n;
    return mean;
}

void calcul_metrique(processus_t *p, int n) {
    for (int i = 0; i < n; i++) {
        p[i].temps_restitution = p[i].temps_fin_execution -
            p[i].temps_arrivee;
    }
}

resultats_t calcul_resultats(processus_t *p, int n, int temps_total, int temps_n_occupation) {
    resultats_t resultats = init_resultats();

    resultats.moyenne_attente = temps_attente_moyenne(p, n);
    float mean_restitution = 0;
    float mean_reponse = 0;
    for (int i = 0; i < n; i++) {
        mean_restitution += p[i].temps_restitution;
        mean_reponse += p[i].temps_reponse;
    }
    mean_restitution /= n;
    mean_reponse /= n;
    resultats.moyenne_restitution = mean_restitution;
    resultats.moyenne_reponse = mean_reponse;
    resultats.taux_occupation = (float) (temps_total - temps_n_occupation)/temps_total;

    return resultats;
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

processus_t *lireFichier(char* file, int *n) {
    printf("ouverture fichier : %s\n", file);

    char buffer[256];
    FILE *f = fopen(file, "r");

    if (!f) {
        perror("Erreur d'ouvrir le fichier");
        exit(-1);
    }

    // printf("fichier ouvert\n");

    fgets(buffer, sizeof(buffer), f);
    // printf("premiere ligne lue : %s", buffer);

    *n = strtol(buffer, NULL, 10);
    // printf("n = %d\n", *n);

    processus_t* p = allocProcessus(*n);
    // printf("allocation ok\n");

    for (int i = 0; i < *n; i++) {
        // printf("lecture processus %d\n", i);
        fgets(buffer, sizeof(buffer), f);
        // printf("ligne : %s", buffer);
        p[i].bursts = malloc(MAX_BURSTS * sizeof(int));

        char *ptr = buffer;
        char *fin;
        int cpt = 0;

        while (*ptr != '\n' && *ptr != '\0') {
            if (cpt == 0) {
                p[i].temps_arrivee = strtol(ptr, &fin, 10);
            }else {
                p[i].bursts[cpt-1] = strtol(ptr, &fin, 10);
            }

            if (fin == ptr) break;

            cpt++;
            ptr = fin;
        }

        p[i].nb_bursts = cpt - 1;
        // printf("Processus %d : temps_arrivee=%d nb_bursts=%d\n", i, p[i].temps_arrivee, p[i].nb_bursts);
        // for (int j = 0; j < p[i].nb_bursts; j++) {
        //     printf("  burst[%d] = %d\n", j, p[i].bursts[j]);
        // }
        initialiser_processus(&p[i]);
    }

    fclose(f);
    return p;
}