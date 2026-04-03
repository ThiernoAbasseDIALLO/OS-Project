/**
 * @file main.c
 * @brief Point d'entree du simulateur d'ordonnancement de processus.
 *
 * Usage :
 *   ./simulateur <fichier_processus> <algo> [quantum]
 *
 * Algorithmes disponibles :
 *   fifo          Premier arrive, premier servi (non preemptif)
 *   sjf           Plus court burst en premier (non preemptif)
 *   sjrf          Plus court burst restant en premier (preemptif)
 *   rr <quantum>  Round Robin avec quantum en ms (preemptif)
 *
 * Format du fichier d'entree (une ligne par processus) :
 *   PID  Arrivee  CPU1  [IO1  CPU2  IO2  ...]
 * Les lignes commencant par '#' et les lignes vides sont ignorees.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrice.h"
#include "output.h"
#include "scheduler.h"

extern scheduler_t SCHEDULERS[];
extern int NB_SCHEDULERS;

#define MAX_BURSTS    20
#define MAX_PROCESSES 64
#define MAX_TEMPS 100

/** @brief Affiche l'aide d'utilisation. */
static void afficher_aide(const char *prog)
{
    printf("Usage: %s <fichier> <algo> [quantum]\n\n", prog);
    printf("  Algorithmes :\n");
    printf("    fifo         Premier arrive, premier servi\n");
    printf("    sjf          Plus court burst en premier (non preemptif)\n");
    printf("    sjrf         Plus court burst restant (preemptif)\n");
    printf("    rr <quantum> Round Robin, quantum en ms (ex: rr 4)\n\n");
    printf("  Exemple : %s data/test.txt rr 4\n", prog);
}

/**
 * @brief Point d'entree principal.
 */
int main(int argc, char *argv[])
{
    if (argc < 3) { afficher_aide(argv[0]); return 1; }

    // printf("debut main\n");

    char *fichier = argv[1];
    const char *algo    = argv[2];

    int n;
    // printf("avant lireFichier\n");
    processus_t* p = lireFichier(fichier, &n);
    etat_processus_t **gantt = allocMat(n, MAX_TEMPS);
    initMat(MAX_TEMPS, n, gantt);

    printf("=== Simulation : %d processus, algorithme %s ===\n\n", n, algo);

    resultats_t r = init_resultats();

    scheduler_t *choix = NULL;

    for (int i = 0; i < NB_SCHEDULERS; i++) {
        if (strcmp(algo, SCHEDULERS[i].nom) == 0) {
            choix = &SCHEDULERS[i];
            break;
        }
    }

    if (!choix) {
        fprintf(stderr, "Algorithme inconnu : %s\n", algo);
        return 1;
    }

    int quantum = 0;
    if (choix->besoin_quantum) {
        if (argc < 4) {
            fprintf(stderr, "Cet algo nécessite un quantum\n");
            return 1;
        }
        quantum = atoi(argv[3]);
    }

    choix->fonction(p, n, quantum, &r, gantt);

    afficher_resultats(p, n, r);
    afficher_gantt(gantt, p, n, r.t_max);
    gantt = libMat(n, gantt);

    exporter_csv(algo, r);
    return 0;
}
