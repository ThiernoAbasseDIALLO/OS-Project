#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

/**
 * @brief Structure d'une Cellule de la file.
 * @details Chaque cellule contient un pointeur vers un processus et
 * un pointeur vers la cellule suivante dans la chaîne.
 */
typedef struct cellule{
    processus_t *elements;      /**< Le processus stocké dans cette cellule */
    struct cellule *suivant;    /**< Pointeur vers la cellule suivante */
}*Liste;

/**
 * @brief Structure de contrôle de la File.
 * @details Utilise deux pointeurs (tete et queue) pour permettre un
 * enfilement et un défilement en temps constant O(1).
 */
typedef struct {
    Liste tete;
    Liste queue;
}File;

/* --- Fonctions de gestion de la mémoire --- */

/**
 * @brief Fait l'allocation mémoire pour une nouvelle cellule.
 * @return Retourne un pointeur vers une cellule (Liste).
 */
Liste allocMem();

void libMem(Liste *l);

/* --- Fonctions de manipulation de la file --- */
/**
 * @brief Permet de tester si une Liste est vide ou non.
 * @param l Une liste en entrée
 * @return Vrai si la liste est vide sinon faux
 */
bool estVideL(Liste l);

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
 * @return Processus le premier processus de la file.
 */
processus_t* sommetF(File f);

#endif
