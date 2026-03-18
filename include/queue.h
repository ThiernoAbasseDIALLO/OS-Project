#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>


typedef struct cellule{
    processus_t *elements;  // Pointeur vers le processus stocké dans la cellule
    struct cellule *suivant;
}*Liste;

/**
 * Soit de cette structure tête et queue pour nous faciliter les opértions d'enfiler et de défiler.
 */
typedef struct {
    Liste tete;
    Liste queue;
}File;

/**
 * @brief Permet de tester si une Liste est vide ou non.
 * @param l Une liste en entrée
 * @return Vrai si la liste est vide sinon faux
 */
bool estVideL(Liste l);

/**
 *  @brief Fait l'allocation mémoire pour une nouvelle cellule.
 * @return Retourne un pointeur vers une cellule (Liste).
 */
Liste allocMem();

void libMem(Liste *l);

/**
 * @brief Initialise une Liste à NULL
 * @return Une liste vide.
 */
Liste initL();

/**
 * @brief Permet de tester si la file est vide ou pas.
 * @param f Une File en entrée.
 * @return Vrai si la file est vide sinon faux.
 */
bool estVideF(File f);

/**
 * @brief Permet d'initialiser une File.
 * @param f Un pointeur vers une File.
 */
void initF(File *f);

/**
 * @brief Permet d'ajouter en queue un nouveau processus.
 * @param f Une file f en entrée et sortie.
 * @param p Un tableau de processus.
 */
void enfiler(File *f, processus_t *p);

/**
 * @brief Permet de supprimer un processus déjà traité, qui est en tête.
 * @param f Une file en entrée et sortie.
 */
void defiler(File *f);

/**
 * @brief Permet d'avoir le premier processus celui qui est en tête de la file.
 * @param f Une File f.
 * @return Retourne le premier processus de la file.
 */
processus_t* sommetF(File f);

#endif
