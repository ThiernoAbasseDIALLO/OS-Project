#ifndef PROCESS_H
#define PROCESS_H

/**
 * @file process.h
 * @brief Définitions des structures et prototypes pour la gestion des processus.
 */

/**
 * @brief États possibles d'un processus dans le cycle de vie de l'ordonnancement.
 */
typedef enum {
    ETAT_NOUVEAU,        /**< Processus chargé, mais pas encore arrivé dans le système */
    ETAT_PRET,           /**< En file d'attente, prêt à être exécuté par le CPU */
    ETAT_EN_EXECUTION,   /**< En train d'utiliser le processeur (CPU Burst) */
    ETAT_EN_ATTENTE,     /**< En train d'effectuer une Entrée/Sortie (I/O Burst) */
    ETAT_TERMINE         /**< Exécution complètement terminée */
} etat_processus_t;

/**
 * @brief Structure représentant un processus et ses métriques.
 */
typedef struct {
    int pid;                        /**< Identifiant unique du processus */
    int temps_arrivee;              /**< Instant d'arrivée dans le système (t) */

    int *bursts;                    /**< Tableau alternant CPU / I/O (ex: [CPU, IO, CPU]) */
    int nb_bursts;                  /**< Nombre total de segments dans le tableau bursts */
    int index_burst_courant;        /**< Indice du segment en cours d'exécution */

    int temps_cpu_restant;          /**< Temps restant pour le burst CPU actuel */
    int temps_io_restant;           /**< Temps restant pour le burst I/O actuel */

    etat_processus_t etat;          /**< État courant du processus */

    /* Indicateurs de performance */
    int first_run;                  /**< Flag : 0 si jamais exécuté, 1 sinon */
    int temps_debut_execution;      /**< Instant du tout premier accès au CPU */
    int temps_fin_execution;        /**< Instant où le processus passe à ETAT_TERMINE */

    int temps_attente;              /**< Cumul du temps passé en ETAT_PRET */
    int temps_reponse;              /**< Délai entre l'arrivée et la première exécution */
    int temps_restitution;          /**< Délai total entre l'arrivée et la fin (Turnaround) */

    int dernier_entree_pret;        /**< Instant de la dernière mise en file READY */
} processus_t;

/**
 * @brief Structure regroupant les résultats globaux de la simulation.
 */
typedef struct {
    float moyenne_attente;          /**< Moyenne des temps d'attente de tous les processus */
    float moyenne_reponse;          /**< Moyenne des temps de réponse */
    float moyenne_restitution;      /**< Moyenne des temps de restitution (Turnaround) */
    float taux_occupation;          /**< Ratio (temps_total - idle) / temps_total */
    int t_max;                      /**< Temps final de la simulation */
} resultats_t;

/* --- Prototypes --- */

/**
 * @brief Initialise les indicateurs de performance d'un processus.
 */
void initialiser_processus(processus_t *p);

/**
 * @brief Alloue dynamiquement un tableau de processus.
 * @param n Nombre de processus à allouer.
 */
processus_t *allocProcessus(int n);

/**
 * @brief Calcule la moyenne du temps d'attente.
 * @return float Moyenne calculée.
 */
float temps_attente_moyenne(processus_t *p, int n);

/**
 * @brief Calcule le temps de restitution (Turnaround) pour chaque processus.
 * @details Formule : temps_fin_execution - temps_arrivee.
 */
void calcul_metrique(processus_t *p, int n);

/**
 * @brief Compile toutes les métriques finales dans la structure resultats_t.
 */
resultats_t calcul_resultats(processus_t *p, int n, int temps_total, int temps_n_occupation);

/**
 * @brief Charge les processus à partir d'un fichier texte.
 * @param nom Chemin du fichier.
 * @param n Pointeur pour récupérer le nombre de processus lus.
 */
processus_t *lireFichier(char* nom, int *n);

/**
 * @brief Initialise une structure de résultats à zéro.
 */
resultats_t init_resultats();

#endif