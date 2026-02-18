#ifndef CSV_H
#define CSV_H

#include "process.h"

void exporter_csv(const char *nom_fichier,
                  processus_t *liste,
                  int n,
                  int temps_total,
                  int temps_cpu_occupe);

#endif
