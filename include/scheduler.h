#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

typedef void (*algo_fn)(
    processus_t *processus,
    int n,
    int quantum,              // ignoré si non RR
    resultats_t *res,
    etat_processus_t **gantt
);

typedef struct {
    const char *nom;   // "fifo", "sjf", ...
    int besoin_quantum;
    algo_fn fonction;
} scheduler_t;

#endif
