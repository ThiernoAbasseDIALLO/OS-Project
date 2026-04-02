/**
 * @file queue.c
 * @brief Implémentation d'une file (FIFO) basée sur une liste chaînée.
 *
 * Participation : 33% pour les 3 auteurs (DIALLO, DOSSO, MAREGA).
 */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

bool estVideL(Liste l) {
    return l==NULL;
}

Liste allocMem() {
    Liste l;
    l = (Liste)malloc(sizeof(struct cellule));

    if (l==NULL) {
        printf("Erreur d'allocation\n");
    }else {
        l->elements = NULL;
        l->suivant = NULL;
    }

    return l;
}

void libMem(Liste *l) {
    /* Libération sécurisée d'une cellule */
    if (estVideL(*l)) {
        free(*l);
        *l = NULL;
    }
}

Liste initL() {
    return NULL;
}

bool estVideF(File f) {
    /* Une file est vide si sa tête (et donc sa queue) est NULL */
    return estVideL(f.tete) && estVideL(f.queue);
}

void initF(File *f) {
    f->tete = initL();
    f->queue = initL();
}

void enfiler(File *f, processus_t *p) {
    Liste cel = allocMem();
    if (cel == NULL) return;

    cel->elements = p;
    cel->suivant = NULL;

    if (estVideF(*f))
        /* Premier élément : la tête et la queue pointent sur lui */
        f->tete = cel;
    else
        /* On attache à la suite de l'ancienne queue */
        f->queue->suivant = cel;

    /* La nouvelle cellule devient la queue officielle */
    f->queue = cel;
}

void defiler(File *f) {
    if (!estVideF(*f)) {
        Liste cel = f->tete;
        if (f->tete == f->queue) {
            /* Cas où il n'y avait qu'un seul élément */
            f->tete = f->queue = NULL;
        }else {
            /* La tête avance à la cellule suivante */
            f->tete = f->tete->suivant;
        }
        libMem(&cel);
    }
}

processus_t* sommetF(File f) {
    if (!estVideF(f)) {
        return f.tete->elements;
    }

    return NULL;
}