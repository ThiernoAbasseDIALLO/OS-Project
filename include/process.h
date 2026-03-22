#ifndef PROCESS_H
#define PROCESS_H

/**
 *  Structure de process, plus les prototypes communs à toutes les algos
 */

/* Définition des états possibles  */
typedef enum {
    ETAT_NOUVEAU,        // Processus créé mais pas encore arrivé
    ETAT_PRET,           // En file des prêts
    ETAT_EN_EXECUTION,   // En cours d’exécution CPU
    ETAT_EN_ATTENTE,     // En attente d’E/S
    ETAT_TERMINE         // Exécution terminée
} etat_processus_t;

typedef struct {
    int pid;                        // Identifiant du processus
    int temps_arrivee;              // Instant ou le processus arrive dans le système

    int *bursts;                    // Tableau alternant CPU / I/O
    int nb_bursts;                  // Nombre total de bursts
    int index_burst_courant;        // Position actuelle dans le tableau bursts

    int temps_cpu_restant;          // Temps CPU restant pour le burst courant
    int temps_io_restant;           // Temps I/O restant si en attente

    etat_processus_t etat;          // Etat courant du processus

    /* Indicateurs de performance */

    int first_run;                  // 0 = jamais exécuté, 1 = déjà exécuté
    int temps_debut_execution;      // Premier instant d’exécution CPU
    int temps_fin_execution;        // Instant de terminaison

    int temps_attente;              // Temps total passé en file des prêts
    int temps_reponse;              // Premier temps de réponse
    int temps_restitution;          // Turnaround time

    int dernier_entree_pret;        // Dernier instant d’entrée en file READY

} processus_t;

typedef struct {
    float moyenne_attente;
    float moyenne_reponse;
    float moyenne_restitution;
    float taux_occupation;
} resultats_t;

resultats_t init_resultats();

void initialiser_processus(processus_t *p);

processus_t *allocProcessus(int n);

/**
 * @brief La fonction calcule le temps moyen des processus en attente,
 * en additionnant tous les temps d'attente des processus.
 * @param p Tableau de processus en entrée
 * @param n Nombre total de processus en entrée
 * @return Un entier qui est le temps moyen des processus en attente.
 */
float temps_attente_moyenne(processus_t *p, int n);

void calcul_metrique(processus_t *p, int n);

resultats_t calcul_resultats(processus_t *p, int n, int temps_total, int temps_n_occupation);

processus_t *lireFichier(char* nom, int *n);

#endif