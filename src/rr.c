/**
 * @file rr.c
 * @brief Logique de l'algorithme d'ordonnancement Round Robin (RR).
 *
 * Stratégie : chaque processus reçoit le CPU pour un quantum fixe.
 * Si son burst n'est pas terminé à l'issue du quantum, il est
 * interrompu et replacé EN FIN de file des prêts (préemptif).
 * Les E/S sont parallélisées.
 *
 * Utilise la File de queue.h pour gérer la file des prêts.
 *
 * Participation : Auteur1 (33%), Auteur2 (33%), Auteur3 (33%)
 */

#include <stdlib.h>
#include "rr.h"
#include "process.h"
#include "queue.h"


/* --------------------------------
 * Fonction exportée : run_rr
 * ---------------------------------*/

/**
 * @brief Exécute l'algorithme Round Robin tick par tick.
 *
 * run_rr() :
 *  1. Repasse EN_EXECUTION → PRET (préparation du tick).
 *  2. Enfile les processus qui arrivent à t.
 *  3. Décrémente les E/S ; si terminées → enfile en fin ou TERMINE.
 *  4. Si CPU libre → prend la tête de file, réinitialise quantum_restant.
 *     Premier accès → enregistre temps_reponse.
 *     Décrémente temps_cpu_restant ET quantum_restant.
 *     Si burst fini → E/S suivante ou TERMINE.
 *     Si quantum épuisé avant fin burst → remet en FIN de file (préemption).
 *  5. Incrémente temps_attente des processus encore PRET.
 *  6. t += 1.
 *
 *   - Sélection = toujours la tête de file (FIFO circulaire).
 *   - Préemption : quand quantum_restant == 0, le processus retourne
 *     en FIN de file via enfiler() et reçoit un nouveau quantum
 *     au prochain tour.
 *
 * @param processus     Tableau des processus à ordonnancer.
 * @param n             Nombre de processus.
 * @param quantum       Durée du quantum en ms (doit être > 0).
 * @param resultats     Pointeur vers la structure de résultats à remplir.
 */
void run_rr(processus_t *processus, int n, int quantum, resultats_t *resultats)
{
    int t                    = 0;
    int termines             = 0;
    int temps_non_occupation = 0;
    int quantum_restant      = 0;

    processus_t *en_execution = NULL;

    File file_prets;
    initF(&file_prets);

    while (termines < n) {

        /* ── Étape 1 : repasser EN_EXECUTION → PRET ── */
        /*
         * En RR, si le quantum n'est pas épuisé et que le burst n'est
         * pas terminé, le processus reprend le CPU au tick suivant.
         * L'étape 4 gère la préemption (quantum == 0) séparément.
         */
        for (int m = 0; m < n; m++) {
            if (processus[m].etat == ETAT_EN_EXECUTION)
                processus[m].etat = ETAT_PRET;
        }

        /* ── Étape 2 : nouvelles arrivées → enfiler en fin ── */
        for (int i = 0; i < n; i++) {
            if (processus[i].temps_arrivee == t) {
                processus[i].etat = ETAT_PRET;
                enfiler(&file_prets, &processus[i]);
            }
        }

        /* ── Étape 3 : décrémenter les E/S en cours ── */
        for (int j = 0; j < n; j++) {
            if (processus[j].etat == ETAT_EN_ATTENTE) {
                processus[j].temps_io_restant--;
                if (processus[j].temps_io_restant == 0) {
                    int index = ++processus[j].index_burst_courant;
                    if (index < processus[j].nb_bursts) {
                        /* Burst suivant = CPU → retour en FIN de file */
                        processus[j].temps_cpu_restant =
                            processus[j].bursts[index];
                        processus[j].etat = ETAT_PRET;
                        enfiler(&file_prets, &processus[j]);
                    } else {
                        /* Dernier burst était une E/S → terminé */
                        processus[j].temps_fin_execution = t;
                        processus[j].etat = ETAT_TERMINE;
                        termines++;
                    }
                }
            }
        }

        /* ── Étape 4 : sélection RR et exécution ── */

        /*
         * Si le quantum est épuisé au tick précédent, le processus
         * a déjà été remis en FIN de file à l'étape 4 du tick précédent.
         * On prend donc la nouvelle tête de file.
         */
        if (en_execution == NULL && !estVideF(file_prets)) {
            en_execution   = sommetF(file_prets);
            defiler(&file_prets);
            quantum_restant = quantum;  /* Nouveau quantum complet */

            /* Premier accès CPU → enregistrer temps de réponse */
            if (en_execution->first_run == 0) {
                en_execution->temps_reponse         = t;
                en_execution->temps_debut_execution = t;
                en_execution->first_run             = 1;
            }
        }

        if (en_execution == NULL) {
            /* CPU idle */
            temps_non_occupation++;
        } else {
            en_execution->etat = ETAT_EN_EXECUTION;
            en_execution->temps_cpu_restant--;
            quantum_restant--;

            if (en_execution->temps_cpu_restant == 0) {
                /* Burst CPU terminé avant ou en même temps que le quantum */
                en_execution->index_burst_courant++;

                if (en_execution->index_burst_courant == en_execution->nb_bursts) {
                    /* Plus de burst → terminé */
                    en_execution->temps_fin_execution = t + 1;
                    en_execution->etat = ETAT_TERMINE;
                    termines++;
                } else {
                    /* Burst suivant = E/S */
                    en_execution->temps_io_restant =
                        en_execution->bursts[en_execution->index_burst_courant];
                    en_execution->etat = ETAT_EN_ATTENTE;
                }
                en_execution    = NULL;
                quantum_restant = 0;

            } else if (quantum_restant == 0) {
                /*
                 * Quantum épuisé, burst non terminé → PRÉEMPTION.
                 * Le processus retourne en FIN de file des prêts.
                 * Il recevra un nouveau quantum complet au prochain tour.
                 */
                en_execution->etat = ETAT_PRET;
                enfiler(&file_prets, en_execution);
                en_execution = NULL;
            }
        }

        /* ── Étape 5 : accumuler l'attente des processus PRET ── */
        for (int l = 0; l < n; l++) {
            if (processus[l].etat == ETAT_PRET)
                processus[l].temps_attente++;
        }

        t++;
    }

    calcul_metrique(processus, n);
    *resultats = calcul_resultats(processus, n, t, temps_non_occupation);
}
