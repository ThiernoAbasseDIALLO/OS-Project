
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "scheduler.h"
#include "simulateur.h"

#define MAX_BURSTS 10   // nombre max de CPU/IO bursts par processus
#define MAX_PROCESSES 20

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <fichier_processus> <algo: fifo|sjf>\n", argv[0]);
        return 1;
    }

    const char *fichier = argv[1];
    const char *algo = argv[2];

    FILE *f = fopen(fichier, "r");
    if (!f) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    processus_t liste[MAX_PROCESSES];
    int bursts[MAX_PROCESSES][MAX_BURSTS]; // stockage temporaire des bursts
    int nb_bursts[MAX_PROCESSES];
    int n = 0;

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f) && n < MAX_PROCESSES) {
        if (ligne[0] == '#' || strlen(ligne) < 2) continue; 

        int pid, arrivee;
        int count = 0;

        char *token = strtok(ligne, " \t\n");
        if (!token) continue;
        pid = atoi(token);

        token = strtok(NULL, " \t\n");
        if (!token) continue;
        arrivee = atoi(token);

        nb_bursts[n] = 0;
        while ((token = strtok(NULL, " \t\n")) && nb_bursts[n] < MAX_BURSTS) {
            bursts[n][nb_bursts[n]++] = atoi(token);
        }

        liste[n].pid = pid;
        liste[n].temps_arrivee = arrivee;
        liste[n].bursts = malloc(sizeof(int) * nb_bursts[n]);
        liste[n].nb_bursts = nb_bursts[n];
        for (int i = 0; i < nb_bursts[n]; i++)
            liste[n].bursts[i] = bursts[n][i];

        initialiser_processus(&liste[n]);
        n++;
    }
    fclose(f);

    ordonnanceur_t ordonnanceur = NULL;
    if (strcmp(algo, "fifo") == 0)
        ordonnanceur = ordonnanceur_fifo;
    else if (strcmp(algo, "sjf") == 0)
        ordonnanceur = ordonnanceur_sjf;
    else {
        fprintf(stderr, "Algorithme inconnu: %s\n", algo);
        return 1;
    }

    // Appel à simuler avec le nom de l'algorithme pour le CSV
    simuler(liste, n, ordonnanceur, algo);

    printf("Simulation terminee avec %s. Resultats dans resultats_%s.csv\n", algo, algo);

    // libérer la mémoire des bursts
    for (int i = 0; i < n; i++)
        free(liste[i].bursts);

    return 0;
}
