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
    if (estVideL(*l)) {
        free(l);
    }
}

Liste initL() {
    return NULL;
}

bool estVideF(File f) {
    return estVideL(f.tete) && estVideL(f.queue);
}

void initF(File *f) {
    f->tete = initL();
    f->queue = initL();
}

void enfiler(File *f, processus_t *p) {
    Liste cel = allocMem();
    cel->elements = p;
    cel->suivant = NULL;
    if (estVideF(*f))
        f->tete = cel;
    else
        f->queue->suivant = cel;
    f->queue = cel;
}

void defiler(File *f) {
    if (!estVideF(*f)) {
        Liste cel = f->tete;
        if (f->tete == f->queue)
            f->tete = f->queue = NULL;
        else
            f->tete = f->tete->suivant;

        libMem(&cel);
    }
}

processus_t* sommetF(File f) {
    if (!estVideF(f)) {
        return f.tete->elements;
    }

    return NULL;
}