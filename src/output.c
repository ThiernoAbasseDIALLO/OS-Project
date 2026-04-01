/**
 * @file output.c
 * @brief Fonctions d'affichage et d'exportation des données de simulation.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdio.h>
#include "output.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SIZE 1024
/*
 * Export des résultats de simulation au format CSV.
 * Compatible tableur (Excel, LibreOffice).
 */

void exporter_csv(const char *nom_algo, resultats_t r) {
    int desc;
    char buf[SIZE];
    char filename[256];
    int n;
    time_t t;
    char date[20];

    /* Génération d'un timestamp pour éviter d'écraser les fichiers précédents */
    t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(date, sizeof(date), "%y%m%d_%H%M", tm_info);

    /* Creer le nom du fichier */
    snprintf(filename, sizeof(filename), "resultats_%s_%s.csv", nom_algo, date);

    /* Création du fichier avec l'appel système open */
    // O_EXCL assure que le fichier ne sera pas écrasé s'il existe déjà
    desc = open(filename, O_WRONLY | O_CREAT | O_EXCL, 644);
    if (desc == -1) {
        perror("Ouverture du fichier de destination impossible");
        close(desc);
    }

    /* 3. Écriture des données formatées par blocs via write */
    // En-tête
    n = snprintf(buf, SIZE, "Indicateur, Valeur en (ms)\n");
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    // Données (Attente, Réponse, Restitution, Occupation)
    n = snprintf(buf, SIZE, "Temps moyen d'attente, %.2f\n", r.moyenne_attente);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    n = snprintf(buf, SIZE, "Temps de réponse moyen, %.2f\n", r.moyenne_reponse);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    n = snprintf(buf, SIZE, "Temps de restitution moyen, %.2f\n", r.moyenne_restitution);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    n = snprintf(buf, SIZE, "Temps d'occupation CPU, %.2f\n", r.taux_occupation);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    close(desc);
    printf("Exportation des résultats réussi, %s\n", filename);
}

void afficher_resultats(processus_t *p, int n, resultats_t r) {
    /* Affichage console simple formaté en colonnes */
    printf("\n--- RESULTATS DE LA SIMULATION ---\n");
    printf("pid, t_arr, t_att, t_rep, t_rest, t_fin\n");

    for (int i = 0; i < n; i++) {
        printf("%d, %d, %d, %d, %d, %d\n",
            (i+1), p[i].temps_arrivee , p[i].temps_attente,
            p[i].temps_reponse, p[i].temps_restitution, p[i].temps_fin_execution);
    }

    printf("----------------------------------\n");

    printf("Moy_att , %.2f\n", r.moyenne_attente);
    printf("Moy_rest , %.2f\n", r.moyenne_restitution);
    printf("Moy_rep , %.2f\n", r.moyenne_reponse);
    printf("T_occ , %.2f\n", r.taux_occupation);
}

void remplir_gantt(etat_processus_t **gantt, processus_t *processus, int n, int t) {
    /* Capture l'état de chaque processus pour le tick 't' */
    for (int i=0; i<n; i++) {
        if (gantt[i][t] == ETAT_NOUVEAU)
            gantt[i][t] = processus[i].etat;
    }
}

void afficher_gantt(etat_processus_t **gantt, processus_t *processus,int n, int t_max) {
    /* Affichage visuel : UC = CPU, ES = Entrée/Sortie, W = Wait (Prêt) */
    printf("Temps\t");
    for (int t=0; t<t_max; t++) {
        printf("%d\t", t);
    }

    for (int i=0; i<n; i++) {
        printf("\nP%d\t", (i+1));
        for (int t=0; t<t_max; t++) {
            switch (gantt[i][t]) {
                case ETAT_EN_EXECUTION:
                    printf("%s\t", "UC");
                    break;
                case ETAT_EN_ATTENTE:
                    printf("%s\t", "ES");
                    break;
                case ETAT_PRET:
                    printf("%s\t", "W");
                    break;
                default:
                    printf(" \t");
                    break;                    
            }
        }
    }

    printf("\nCPU\t");
    for (int t=0; t<t_max; t++) {
        int cpu_occupe = 0;
        for (int i = 0; i < n; i++) {
            if (gantt[i][t] == ETAT_EN_EXECUTION) {
                cpu_occupe = 1;
                break;
            }
        }
        printf("%s\t", cpu_occupe ? "###" : "   ");
    }
    printf("\n");
}