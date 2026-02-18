#include "process.h"

/*
 * Initialisation complète d’un processus.
 * Cette fonction doit être appelée après le chargement
 * des données (PID, temps d’arrivée, bursts, nb_bursts).
 */

void initialiser_processus(processus_t *p)
{
    /* Positionnement initial */
    p->index_burst_courant = 0;

    /* Initialement, le premier burst est toujours CPU */
    p->temps_cpu_restant = p->bursts[0];

    /* Aucun I/O actif au départ */
    p->temps_io_restant = 0;

    /* Etat initial */
    p->etat = ETAT_NOUVEAU;

    /* Indicateurs de performance */
    p->temps_debut_execution = -1;   
    p->temps_fin_execution = 0;

    p->temps_attente = 0;
    p->temps_reponse = -1;           
    p->temps_restitution = 0;

    p->dernier_entree_pret = 0;
}
