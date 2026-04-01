#ifndef OUTPUT_H
#define OUTPUT_H

#include "process.h"

/**
 * @brief Exporte les métriques de simulation dans un fichier CSV.
 * @note Le nom du fichier est généré automatiquement : resultats_ALGO_DATE.csv.
 * @param[in] nom_algo Nom de l'algorithme (utilisé pour le nom du fichier).
 * @param[in] r        Structure contenant les moyennes et taux calculés.
 */
void exporter_csv(const char *nom_algo, resultats_t r);

/**
 * @brief Affiche un tableau récapitulatif des processus sur la console.
 * @param[in] p  Tableau des processus avec leurs métriques individuelles.
 * @param[in] n  Nombre de processus.
 * @param[in] r  Métriques globales (moyennes).
 */
void afficher_resultats(processus_t *p, int n, resultats_t r);

/**
 * @brief Enregistre l'état de chaque processus à un instant T dans la matrice.
 * @param[out] gantt     Matrice à remplir.
 * @param[in]  processus Tableau des processus à l'instant t.
 * @param[in]  n         Nombre de processus.
 * @param[in]  t         Instant actuel de la simulation.
 */
void remplir_gantt(etat_processus_t **gantt, processus_t *processus, int n, int t);

/**
 * @brief Affiche visuellement le diagramme de Gantt dans le terminal.
 * @details Affiche une ligne par processus avec des codes (UC, ES, W) et
 * une ligne récapitulative de l'occupation CPU.
 * @param[in] gantt  Matrice d'états complète.
 * @param[in] processus Utilisé pour les noms des processus.
 * @param[in] n      Nombre de processus.
 * @param[in] t_max  Durée totale de la simulation.
 */
void afficher_gantt(etat_processus_t **gantt, processus_t *processus,int n, int t_max);

#endif
