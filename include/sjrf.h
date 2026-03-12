#ifndef SJRF_H
#define SJRF_H

#include "process.h"

/**
 * @brief Lance la simulation de l'algorihtme SJRF
 * @param processus Tableau des processus à ordonnancer
 * @param n Nombre de processus
 */
void run_sjrf(processus_t *processus, int n);

#endif //SJRF_H