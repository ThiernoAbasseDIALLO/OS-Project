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

#include "process.h"
#include "sjrf.h"
#include "rr.h"
#include "fifo.h"
#include "sjf.h"
#include "output.h"
#include "matrice.h"

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

    printf("debut main\n");

    char *fichier = argv[1];
    const char *algo    = argv[2];

    int n;
    printf("avant lireFichier\n");
    processus_t* p = lireFichier(fichier, &n);
    etat_processus_t **gantt = allocMat(n, MAX_TEMPS);
    initMat(MAX_TEMPS, n, gantt);

    printf("=== Simulation : %d processus, algorithme %s ===\n\n", n, algo);

    resultats_t r = init_resultats();

    if (strcmp(algo, "fifo") == 0) {
        run_fifo(p, n, &r, gantt);

    } else if (strcmp(algo, "sjf") == 0) {
        run_sjf(p, n, &r, gantt);

    } else if (strcmp(algo, "sjrf") == 0) {
        run_sjrf(p, n, &r, gantt);

    } else if (strcmp(algo, "rr") == 0) {
        if (argc < 4) {
            fprintf(stderr, "RR : le quantum est obligatoire. Ex: rr 4\n");
            afficher_aide(argv[0]);
            return 1;
        }

        int quantum = atoi(argv[3]);

        if (quantum <= 0) {
            fprintf(stderr, "RR : le quantum doit etre un entier > 0.\n");
            return 1;
        }

        run_rr(p, n, quantum, &r, gantt);

    } else {
        fprintf(stderr, "Algorithme inconnu : '%s'\n\n", algo);
        afficher_aide(argv[0]);
        // for (int i = 0; i < n; i++) free(liste[i].bursts);
        return 1;
    }

    afficher_resultats(p, n, r);
    afficher_gantt(gantt, p, n, r.t_max);
    gantt = libMat(n, gantt);

    // exporter_csv(algo, r);
    return 0;
}
