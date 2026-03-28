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

    t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strftime(date, sizeof(date), "%y%m%d_%H%M", tm_info);

    /* Creer le nom du fichier */
    snprintf(filename, sizeof(filename), "resultats_%s_%s.csv", nom_algo, date);

    /* Ouverture du fichier */
    desc = open(filename, O_WRONLY | O_CREAT | O_EXCL, 644);
    if (desc == -1) {
        perror("Ouverture du fichier de destination impossible");
        close(desc);
    }

    /* Ecriture de l'en-tête du fichier CSV */
    n = snprintf(buf, SIZE, "Indicateur, Valeur en (ms)\n");
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    /* Temps moyen d'attente */
    n = snprintf(buf, SIZE, "Temps moyen d'attente, %.2f\n", r.moyenne_attente);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    /* Temps de reponse moyen */
    n = snprintf(buf, SIZE, "Temps de réponse moyen, %.2f\n", r.moyenne_reponse);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    /* Temps de restitution moyen */
    n = snprintf(buf, SIZE, "Temps de restitution moyen, %.2f\n", r.moyenne_restitution);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    /* Temps d'occupation CPU */
    n = snprintf(buf, SIZE, "Temps d'occupation CPU, %.2f\n", r.taux_occupation);
    if (write(desc, buf, n) == -1) {
        perror("Erreur d'écriture");
        close(desc);
    }

    close(desc);
    printf("Exportation des résultats réussi, %s\n", filename);
}

void afficher_resultats(processus_t *p, int n, resultats_t r) {
    printf("pid, t_arr, t_att, t_rep, t_rest, t_fin\n");

    for (int i = 0; i < n; i++) {
        printf("%d, %d, %d, %d, %d, %d\n",
            (i+1), p[i].temps_arrivee , p[i].temps_attente,
            p[i].temps_reponse, p[i].temps_restitution, p[i].temps_fin_execution);
    }

    printf("Moy_att , %.2f\n", r.moyenne_attente);
    printf("Moy_rest , %.2f\n", r.moyenne_restitution);
    printf("Moy_rep , %.2f\n", r.moyenne_reponse);
    printf("T_occ , %.2f\n", r.taux_occupation);
}