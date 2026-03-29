#ifndef CSV_H
#define CSV_H

#include "process.h"

void exporter_csv(const char *nom_fichier, resultats_t r);

void afficher_resultats(processus_t *p, int n, resultats_t r);

void remplir_gantt(etat_processus_t **gantt, processus_t *processus, int n, int t);

void afficher_gantt(etat_processus_t **gantt, processus_t *processus,int n, int t_max);

#endif
