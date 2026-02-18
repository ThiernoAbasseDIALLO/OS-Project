#include <stdio.h>
#include "csv.h"

/*
 * Export des résultats de simulation au format CSV.
 * Compatible tableur (Excel, LibreOffice).
 */

void exporter_csv(const char *nom_fichier,
                  processus_t *liste,
                  int n,
                  int temps_total,
                  int temps_cpu_occupe)
{
    FILE *f = fopen(nom_fichier, "w");

    if (!f) {
        perror("Erreur ouverture fichier CSV");
        return;
    }

    /* En-tête des colonnes */
    fprintf(f,
        "PID;Temps_arrivee;Temps_attente;Temps_reponse;Temps_restitution\n");

    double somme_attente = 0.0;
    double somme_reponse = 0.0;
    double somme_restitution = 0.0;

    for (int i = 0; i < n; i++)
    {
        fprintf(f,
            "%d;%d;%d;%d;%d\n",
            liste[i].pid,
            liste[i].temps_arrivee,
            liste[i].temps_attente,
            liste[i].temps_reponse,
            liste[i].temps_restitution
        );

        somme_attente += liste[i].temps_attente;
        somme_reponse += liste[i].temps_reponse;
        somme_restitution += liste[i].temps_restitution;
    }

    /* Ligne vide */
    fprintf(f, "\n");

    /* Moyennes */
    fprintf(f,
        "MOYENNE;;%.2f;%.2f;%.2f\n",
        somme_attente / n,
        somme_reponse / n,
        somme_restitution / n
    );

    /* Utilisation CPU */
    double taux_utilisation =
        100.0 * temps_cpu_occupe / temps_total;

    fprintf(f,
        "UTILISATION_CPU;;;;%.2f%%\n",
        taux_utilisation
    );

    fclose(f);
}
