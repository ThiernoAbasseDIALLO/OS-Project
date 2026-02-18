#ifndef PROCESS_H
#define PROCESS_H

/* Définition des états possibles  */


typedef enum {
    ETAT_NOUVEAU,        // Processus créé mais pas encore arrivé
    ETAT_PRET,           // En file des prêts
    ETAT_EN_EXECUTION,   // En cours d’exécution CPU
    ETAT_EN_ATTENTE,     // En attente d’E/S
    ETAT_TERMINE         // Exécution terminée
} etat_processus_t;


/* Structure représentant un processus */


typedef struct {

    int pid;                        // Identifiant du processus
    int temps_arrivee;              // Instant d’arrivée dans le système

    int *bursts;                    // Tableau alternant CPU / I/O
    int nb_bursts;                  // Nombre total de bursts
    int index_burst_courant;        // Position actuelle dans le tableau bursts

    int temps_cpu_restant;          // Temps CPU restant pour le burst courant
    int temps_io_restant;           // Temps I/O restant si en attente

    etat_processus_t etat;          // Etat courant du processus

    /* Indicateurs de performance */

    int temps_debut_execution;      // Premier instant d’exécution CPU
    int temps_fin_execution;        // Instant de terminaison

    int temps_attente;              // Temps total passé en file des prêts
    int temps_reponse;              // Premier temps de réponse
    int temps_restitution;          // Turnaround time

    int dernier_entree_pret;        // Dernier instant d’entrée en file READY

} processus_t;



/* Initialisation d’un processus   */


void initialiser_processus(processus_t *p);

#endif
