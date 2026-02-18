#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "simulateur.h"
#include "queue.h"
#include "csv.h"


  /* Affichage des résultats dans le terminal*/
void afficher_resultats_console(processus_t *liste, int n, int temps_total, int temps_cpu_occupe)
{
    double somme_attente = 0, somme_reponse = 0, somme_restitution = 0;

    printf("\n-------- RESULTATS DE SIMULATION --------\n");
    printf("PID | Arrivee | Attente | Reponse | Restitution\n");

    for (int i = 0; i < n; i++)
    {
        printf("%3d | %7d | %7d | %7d | %11d\n",
               liste[i].pid,
               liste[i].temps_arrivee,
               liste[i].temps_attente,
               liste[i].temps_reponse,
               liste[i].temps_restitution);

        somme_attente += liste[i].temps_attente;
        somme_reponse += liste[i].temps_reponse;
        somme_restitution += liste[i].temps_restitution;
    }

    printf("\nMOYENNES : Attente=%.2f, Reponse=%.2f, Restitution=%.2f\n",
           somme_attente / n,
           somme_reponse / n,
           somme_restitution / n);

    double taux_cpu = 100.0 * temps_cpu_occupe / temps_total;
    printf("Taux d'utilisation CPU : %.2f%%\n", taux_cpu);
    printf("-------------------------------\n\n");
}

/* 
   Déterminer le prochain événement système
   (fin CPU courant ou fin d’un I/O)
   */
static int determiner_prochain_evenement(processus_t *liste,
                                         int n,
                                         processus_t *en_execution)
{
    int delta = INT_MAX;

    /* Fin CPU */
    if (en_execution && en_execution->temps_cpu_restant > 0)
        delta = en_execution->temps_cpu_restant;

    /* Fin d’un I/O */
    for (int i = 0; i < n; i++)
    {
        if (liste[i].etat == ETAT_EN_ATTENTE &&
            liste[i].temps_io_restant > 0 &&
            liste[i].temps_io_restant < delta)
        {
            delta = liste[i].temps_io_restant;
        }
    }

    return delta;
}

/* 
   Simulation générique non-préemptive
    */
void simuler(processus_t *liste,
             int n,
             ordonnanceur_t ordonnanceur,
             const char *nom_algo)
{
    int temps_courant = 0;
    int processus_termines = 0;
    int temps_cpu_occupe = 0;

    processus_t *en_execution = NULL;
    file_t *file_prets = creer_file(n);

    while (processus_termines < n)
    {
        /*  Gestion des arrivées */
        for (int i = 0; i < n; i++)
        {
            if (liste[i].etat == ETAT_NOUVEAU &&
                liste[i].temps_arrivee <= temps_courant)
            {
                liste[i].etat = ETAT_PRET;
                liste[i].dernier_entree_pret = temps_courant;
                enfiler(file_prets, &liste[i]);
            }
        }

        /*  Fin des I/O */
        for (int i = 0; i < n; i++)
        {
            if (liste[i].etat == ETAT_EN_ATTENTE &&
                liste[i].temps_io_restant == 0)
            {
                liste[i].index_burst_courant++;

                if (liste[i].index_burst_courant < liste[i].nb_bursts)
                {
                    liste[i].temps_cpu_restant =
                        liste[i].bursts[liste[i].index_burst_courant];

                    liste[i].etat = ETAT_PRET;
                    liste[i].dernier_entree_pret = temps_courant;
                    enfiler(file_prets, &liste[i]);
                }
            }
        }

        /*  Sélection CPU */
        if (!en_execution && file_prets->taille > 0)
        {
            processus_t *choisi =
                ordonnanceur(file_prets->elements,
                             file_prets->taille);

            int indice = 0;
            while (file_prets->elements[indice] != choisi)
                indice++;

            en_execution = defiler(file_prets, indice);
            en_execution->etat = ETAT_EN_EXECUTION;

            /* Mise à jour du temps d’attente */
            en_execution->temps_attente +=
                temps_courant -
                en_execution->dernier_entree_pret;

            /* Temps de réponse */
            if (en_execution->temps_debut_execution == -1)
            {
                en_execution->temps_debut_execution =
                    temps_courant;

                en_execution->temps_reponse =
                    temps_courant -
                    en_execution->temps_arrivee;
            }
        }

        /*  Avancement du temps */
        int delta = determiner_prochain_evenement(liste, n, en_execution);

        if (delta == INT_MAX)
            break;

        temps_courant += delta;

        /* CPU en cours */
        if (en_execution)
        {
            en_execution->temps_cpu_restant -= delta;
            temps_cpu_occupe += delta;
        }

        /* I/O parallélisées */
        for (int i = 0; i < n; i++)
        {
            if (liste[i].etat == ETAT_EN_ATTENTE)
                liste[i].temps_io_restant -= delta;
        }

        /*  Fin CPU */
        if (en_execution &&
            en_execution->temps_cpu_restant == 0)
        {
            en_execution->index_burst_courant++;

            if (en_execution->index_burst_courant >=
                en_execution->nb_bursts)
            {
                /* Terminé */
                en_execution->etat = ETAT_TERMINE;
                en_execution->temps_fin_execution =
                    temps_courant;

                en_execution->temps_restitution =
                    temps_courant -
                    en_execution->temps_arrivee;

                processus_termines++;
            }
            else
            {
                /* Passage en I/O */
                en_execution->temps_io_restant =
                    en_execution->bursts
                    [en_execution->index_burst_courant];

                en_execution->etat = ETAT_EN_ATTENTE;
            }

            en_execution = NULL;
        }
    }

    /* Création du nom de fichier CSV selon l'algorithme */
    char nom_fichier[64];
    snprintf(nom_fichier, sizeof(nom_fichier),
             "resultats_%s.csv", nom_algo);

    /* Export CSV */
    exporter_csv(nom_fichier, liste, n, temps_courant, temps_cpu_occupe);

    /* Affichage texte dans le terminal */
    afficher_resultats_console(liste, n, temps_courant, temps_cpu_occupe);

    /* Libération de la file */
    liberer_file(file_prets);
}
