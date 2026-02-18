#include <stdio.h>

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Type générique d’un algorithme d’ordonnancement */
typedef processus_t* (*algorithme_ordonnancement_t)(processus_t **file_prets, int taille);

/* FIFO : Premier arrivé, premier servi */
processus_t* ordonnanceur_fifo(processus_t **file_prets, int taille);

/* SJF : Plus court en premier  */
processus_t* ordonnanceur_sjf(processus_t **file_prets, int taille);

#endif
