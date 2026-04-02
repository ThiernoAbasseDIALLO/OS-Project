#include "scheduler.h"
#include "fifo.h"
#include "sjf.h"
#include "sjrf.h"
#include "rr.h"

static void wrapper_fifo(processus_t *p,int n,int q,resultats_t *r,etat_processus_t **g){ run_fifo(p,n,r,g); }
static void wrapper_sjf (processus_t *p,int n,int q,resultats_t *r,etat_processus_t **g){ run_sjf(p,n,r,g); }
static void wrapper_sjrf(processus_t *p,int n,int q,resultats_t *r,etat_processus_t **g){ run_sjrf(p,n,r,g); }

scheduler_t SCHEDULERS[] = {
    {"fifo", 0, wrapper_fifo},
    {"sjf",  0, wrapper_sjf},
    {"sjrf", 0, wrapper_sjrf},
    {"rr",   1, run_rr},
};

int NB_SCHEDULERS = sizeof(SCHEDULERS)/sizeof(scheduler_t);