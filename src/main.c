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
#include "scheduler.h"
#include "simulateur.h"
#include "sjrf.h"
#include "rr.h"

#define MAX_BURSTS    20
#define MAX_PROCESSES 64

// /**
//  * @brief Charge les processus depuis un fichier texte.
//  * @param chemin     Chemin du fichier source.
//  * @param liste      Tableau de sortie (taille MAX_PROCESSES).
//  * @param bursts_tmp Stockage temporaire des bursts lus.
//  * @return Nombre de processus charges, -1 en cas d'erreur.
//  */
// static int charger_processus(const char *chemin,
//                               processus_t *liste,
//                               int bursts_tmp[][MAX_BURSTS])
// {
//     FILE *f = fopen(chemin, "r");
//     if (!f) { perror("Erreur ouverture fichier"); return -1; }
//
//     int n = 0;
//     char ligne[256];
//
//     while (fgets(ligne, sizeof(ligne), f) && n < MAX_PROCESSES) {
//         if (ligne[0] == '#' || strlen(ligne) < 2) continue;
//
//         char *token = strtok(ligne, " \t\n");
//         if (!token) continue;
//         liste[n].pid = atoi(token);
//
//         token = strtok(NULL, " \t\n");
//         if (!token) continue;
//         liste[n].temps_arrivee = atoi(token);
//
//         int nb = 0;
//         while ((token = strtok(NULL, " \t\n")) && nb < MAX_BURSTS)
//             bursts_tmp[n][nb++] = atoi(token);
//
//         if (nb == 0) {
//             fprintf(stderr, "Processus %d sans burst, ignore.\n", liste[n].pid);
//             continue;
//         }
//
//         liste[n].bursts    = malloc(sizeof(int) * nb);
//         if (!liste[n].bursts) { perror("malloc bursts"); fclose(f); return -1; }
//         liste[n].nb_bursts = nb;
//         for (int i = 0; i < nb; i++)
//             liste[n].bursts[i] = bursts_tmp[n][i];
//
//         initialiser_processus(&liste[n]);
//         n++;
//     }
//
//     fclose(f);
//     return n;
// }
//
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

    printf("=== Simulation : %d processus, algorithme %s ===\n\n", n, algo);

    resultats_t r = init_resultats();

    if (strcmp(algo, "fifo") == 0) {
        // simuler(liste, n, ordonnanceur_fifo, "fifo");

    } else if (strcmp(algo, "sjf") == 0) {
        // simuler(liste, n, ordonnanceur_sjf, "sjf");

    } else if (strcmp(algo, "sjrf") == 0) {
        run_sjrf(p, n, &r);

    } else if (strcmp(algo, "rr") == 0) {
        if (argc < 4) {
            fprintf(stderr, "RR : le quantum est obligatoire. Ex: rr 4\n");
            afficher_aide(argv[0]);
            // for (int i = 0; i < n; i++) free(liste[i].bursts);
            return 1;
        }
        int quantum = atoi(argv[3]);
        if (quantum <= 0) {
            fprintf(stderr, "RR : le quantum doit etre un entier > 0.\n");
            // for (int i = 0; i < n; i++) free(liste[i].bursts);
            return 1;
        }
        // simuler_rr(liste, n, quantum);

    } else {
        fprintf(stderr, "Algorithme inconnu : '%s'\n\n", algo);
        afficher_aide(argv[0]);
        // for (int i = 0; i < n; i++) free(liste[i].bursts);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        printf("P%d : fin=%d attente=%d reponse=%d restitution=%d\n",
            i, p[i].temps_fin_execution, p[i].temps_attente,
            p[i].temps_reponse, p[i].temps_restitution);
    }

    printf("Temps moyenne d'attente : %f\n", r.moyenne_attente);
    printf("Temps moyenne de restitution : %f\n", r.moyenne_restitution);
    printf("Temps moyenne de reponse : %f\n", r.moyenne_reponse);
    printf("Taux d'occupation du cpu : %f\n", r.taux_occupation);

    return 0;
}
