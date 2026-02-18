#include <stdio.h>

#include "scheduler.h"

/*
 * FIFO :
 * Le processus choisi est le premier arrivé dans la file des prêts.
 */
processus_t* ordonnanceur_fifo(processus_t **file_prets, int taille)
{
    if (taille == 0)
        return NULL;

    return file_prets[0];
}


/*
 * SJF non-préemptif :
 * On choisit le processus prêt ayant le plus petit temps CPU restant.
 */
processus_t* ordonnanceur_sjf(processus_t **file_prets, int taille)
{
    if (taille == 0)
        return NULL;

    int indice_min = 0;

    for (int i = 1; i < taille; i++)
    {
        if (file_prets[i]->temps_cpu_restant <
            file_prets[indice_min]->temps_cpu_restant)
        {
            indice_min = i;
        }
    }

    return file_prets[indice_min];
}
