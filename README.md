# Simulateur d'ordonnancement de processus

> Simulateur en C (standard C99) implémentant les algorithmes **FIFO**, **SJF**, **SJRF** et **Round Robin** avec gestion des cycles CPU et E/S parallélisées, export CSV et diagramme de Gantt.

**Auteurs** — DIALLO Thierno Abasse · DOSSO Siaka · MAREGA Aboubacar  
**Encadrant** — Jean-Luc Bourdon · L3 Informatique · CY Cergy Paris Université · 2025-2026

---

## Table des matières

1. [Présentation](#1-présentation)
2. [Prérequis & Installation](#2-prérequis--installation)
3. [Compilation & Installation du logiciel](#3-compilation--installation-du-logiciel)
4. [Utilisation](#4-utilisation)
5. [Format du fichier d'entrée](#5-format-du-fichier-dentrée)
6. [Algorithmes implémentés](#6-algorithmes-implémentés)
7. [Indicateurs de performance](#7-indicateurs-de-performance)
8. [Sorties produites](#8-sorties-produites)
9. [Structure du projet](#9-structure-du-projet)
10. [Ajouter un nouvel algorithme](#10-ajouter-un-nouvel-algorithme)
11. [Documentation Doxygen](#11-documentation-doxygen)
12. [Jeux de test fournis](#12-jeux-de-test-fournis)
13. [Répartition du travail](#13-répartition-du-travail)
14. [Références](#14-références)

---

## 1. Présentation

Le simulateur prend en entrée un fichier texte décrivant un ensemble de processus (temps d'arrivée, cycles CPU et cycles E/S alternés) et simule leur exécution **tick par tick** selon l'algorithme d'ordonnancement choisi.

**Ce que produit le simulateur :**
- Affichage console des indicateurs de performance par processus et en moyenne
- Diagramme de Gantt textuel (`UC` / `ES` / `W` par tick)
- Export automatique d'un fichier CSV horodaté, compatible tableur (Excel, LibreOffice)

**Algorithmes disponibles :** `fifo` · `sjf` · `sjrf` · `rr`

---

## 2. Prérequis & Installation

### Système requis

- Linux / Ubuntu 20.04+ (testé sous Ubuntu 22.04 et Debian 12)
- `gcc` ≥ 9.0 avec support C99
- `make`
- `doxygen` (pour la documentation HTML — optionnel)

### Installation des dépendances

```bash
sudo apt update
sudo apt install build-essential make doxygen
```

### Récupérer le projet

```bash
tar -xzf L3_I-OS-Projet-DIALLO-DOSSO-MAREGA.tgz
cd OS-Project/
```

---

## 3. Compilation & Installation du logiciel

### Compilation

```bash
# Compilation complète (produit ./simulateur)
make

# Nettoyage des binaires et objets
make clean

# Recompilation propre
make clean && make
```

Le binaire produit s'appelle `simulateur` et est placé à la racine du projet.

> **Note :** Le Makefile utilise `$(wildcard src/*.c)` — tout nouveau fichier `.c` ajouté dans `src/` est compilé automatiquement sans modification du Makefile.

### Installation du logiciel

Le Makefile propose une cible `install` qui installe le binaire dans `$HOME/bin/` si l'utilisateur n'a pas les droits sur `/usr/local/bin`, ou dans `/usr/local/bin/` sinon :

```bash
make install       # Installe simulateur dans $HOME/bin/ ou /usr/local/bin/
make uninstall     # Désinstalle le binaire
```

### Génération de la documentation Doxygen

```bash
make doc           # Génère la documentation HTML dans doc/html/
# Ouvrir dans un navigateur :
xdg-open doc/html/index.html
```

---

## 4. Utilisation

### Syntaxe

```
./simulateur <fichier_processus> <algorithme> [quantum]
```

| Paramètre | Description |
|---|---|
| `<fichier_processus>` | Chemin vers le fichier d'entrée (voir section 5) |
| `<algorithme>` | `fifo`, `sjf`, `sjrf` ou `rr` |
| `[quantum]` | Entier > 0, **obligatoire uniquement pour `rr`** |

### Exemples complets

```bash
# FIFO — First In, First Out
./simulateur data/test.txt fifo

# SJF — Shortest Job First
./simulateur data/test.txt sjf

# SJRF — Shortest Job Remaining First (préemptif)
./simulateur data/test.txt sjrf

# Round Robin avec quantum de 4 ms
./simulateur data/test.txt rr 4

# Round Robin avec quantum de 2 ms
./simulateur data/test.txt rr 2

# Tester avec d'autres jeux de données
./simulateur data/test1.txt sjrf
./simulateur data/test2.txt rr 3
```

### Afficher l'aide

```bash
./simulateur
```

---

## 5. Format du fichier d'entrée

```
<nombre_de_processus>
<Arrivee>  <CPU1>  [IO1  CPU2  IO2  ...]
<Arrivee>  <CPU1>  [IO1  CPU2  IO2  ...]
...
```

**Règles :**
- La **première ligne** contient uniquement le nombre de processus.
- Chaque ligne suivante décrit un processus.
- Les bursts **alternent CPU et E/S** : `CPU1, IO1, CPU2, IO2, ...`
- Un processus peut n'avoir **qu'un seul burst CPU** (sans E/S).
- Si le nombre de bursts est **pair**, le dernier burst est une E/S — le processus se termine après cette E/S sans repasser par le CPU.
- Toutes les durées sont en **millisecondes (ms)**.

### Exemple — `data/test.txt`

```
2
0  3  2  2  2  2
1  2  2  3  3  2
```

Décodage :

| Processus | Arrive | CPU<sub>1</sub> | E/S<sub>1</sub> | CPU<sub>2</sub> | E/S<sub>2</sub> | CPU<sub>3</sub> |
|---|--------|-----------------|-----------------|---|-------|-------|
| P1 | t=0 ms | 3 ms            | 2 ms            | 2 ms | 2 ms  | 2 ms |
| P2 | t=2 ms | 2 ms            | 3 ms            | 3 ms | 2 ms  | 2 |

### Exemple processus CPU — `data/test2.txt`

```
3
0  5
2  3
4  7
```

Chaque processus n'a qu'un seul burst CPU, sans cycle E/S.

---

## 6. Algorithmes implémentés

### 6.1 FIFO — First In, First Out

| Propriété | Valeur |
|---|---|
| Préemptif | Non |
| Critère de sélection | Ordre d'arrivée |
| Structure utilisée | File chaînée (`queue.h`) |
| Optimal pour | Simplicité, processus de même durée |
| Risque | Effet convoi : un long processus bloque les courts |

Le premier processus arrivé est le premier servi. Non-préemptif : une fois élu, il garde le CPU jusqu'à la fin de son burst.

### 6.2 SJF — Shortest Job First

| Propriété | Valeur |
|---|---|
| Préemptif | Non |
|Critère de sélection | Plus court burst CPU courant |
| Structure utilisée | Parcours du tableau `processus[]` |
| Optimal pour | Minimiser le temps d'attente moyen |
| Risque | Famine pour les processus longs |

Parmi tous les processus prêts, celui dont le burst CPU courant est le plus court est élu. Non-préemptif : un processus plus court arrivant en cours d'exécution devra attendre la fin du burst en cours.

### 6.3 SJRF — Shortest Job Remaining First

| Propriété | Valeur |
|---|---|
| Préemptif | Oui |
| Critère de sélection | Plus court temps CPU restant à chaque tick |
| Structure utilisée | Parcours du tableau `processus[]` |
| Optimal pour | Temps de réponse |
| Risque | Famine + nombreuses préemptions |

Variante préemptive de SJF. À chaque tick, le processus au plus court temps CPU **restant** est sélectionné — même s'il faut interrompre le processus en cours.

### 6.4 RR — Round Robin

| Propriété | Valeur |
|---|---|
| Préemptif | Oui |
| Critère de sélection | Tête de file + quantum configurable |
| Structure utilisée | File chaînée (`queue.h`) |
| Optimal pour | Équité entre processus |
| Risque | Temps de restitution élevé si quantum trop petit |

Chaque processus reçoit le CPU pour un quantum fixe. À l'expiration du quantum, s'il n'a pas terminé son burst, il est remis **en fin de file**. Aucun processus ne peut être en famine.

---

## 7. Indicateurs de performance

Le simulateur calcule les métriques suivantes **par processus** et **en moyenne** :

| Indicateur | Formule | Description |
|---|---|---|
| **Temps d'attente** | Σ ticks en état PRÊT non élu | Temps passé à attendre le CPU |
| **Temps de réponse** | `t_debut_exec − t_arrivee` | Délai avant le 1er accès au CPU |
| **Temps de restitution** | `t_fin − t_arrivee` | Durée totale de vie du processus |
| **Taux d'occupation CPU** | `(t_total − t_idle) / t_total` | Fraction du temps où le CPU est actif |

---

## 8. Sorties produites

### 8.1 Affichage console

```
=== Simulation : 2 processus, algorithme fifo ===

pid, t_arr, t_att, t_rep, t_rest, t_fin
1, 0, 1, 0, 12, 12
2, 1, 2, 2, 14, 15
Moy_att  , 1.50
Moy_rest , 13.00
Moy_rep  , 1.00
T_occ    , 0.93
```

> Les en-têtes de colonnes (`pid, t_arr, t_att, t_rep, t_rest, t_fin`) sont conçus pour être directement utilisables comme labels de graphiques dans un tableur.

### 8.2 Diagramme de Gantt textuel

```
Temps  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
P1    UC  UC  UC  ES  ES  UC  UC  ES  ES   W  UC  UC
P2         W   W  UC  UC  ES  ES  UC  UC  UC  ES  ES  ES  UC  UC
CPU  ###  ###  ###  ###  ###  ###  ###  ###  ###  ###  ###  ###      ###  ###
```

**Légende :**
- `UC` — processus sur le CPU (Using CPU)
- `ES` — processus en E/S parallélisée
- `W`  — processus en attente dans la file des prêts
- `   ` — CPU idle (aucun processus prêt)

### 8.3 Export CSV automatique

Un fichier `resultats_<algo>_<YYMMDD_HHMM>.csv` est généré automatiquement dans le répertoire courant :

```csv
Indicateur, Valeur en (ms)
Temps moyen d'attente, 1.50
Temps de réponse moyen, 1.00
Temps de restitution moyen, 13.00
Temps d'occupation CPU, 0.93
```

Ce fichier est directement importable dans **Excel** ou **LibreOffice Calc** pour générer des graphiques comparatifs entre algorithmes.

---

## 9. Structure du projet

```
OS-Project/
├── src/
│   ├── main.c          Point d'entrée : lecture fichier, dispatch algo, Gantt
│   ├── process.c       Structure processus_t, calcul des métriques, lireFichier()
│   ├── queue.c         File chaînée (initF, enfiler, defiler, sommetF, estVideF)
│   ├── fifo.c          Algorithme FIFO — non-préemptif, File des prêts
│   ├── sjf.c           Algorithme SJF  — non-préemptif, sélection sur tableau
│   ├── sjrf.c          Algorithme SJRF — préemptif, sélection à chaque tick
│   ├── rr.c            Round Robin — préemptif, quantum configurable
│   ├── output.c        Affichage console, Gantt textuel, export CSV
│   ├── matrice.c       Allocation/libération de la matrice de Gantt
│   ├── scheduler.c     Registre des algorithmes et interface d'extensibilité
│   └── tableau.c       Utilitaires tableau
├── include/
│   ├── process.h       Structures processus_t et resultats_t
│   ├── queue.h         Structure File et ses opérations
│   ├── fifo.h          Prototype run_fifo()
│   ├── sjf.h           Prototype run_sjf()
│   ├── sjrf.h          Prototype run_sjrf()
│   ├── rr.h            Prototype run_rr()
│   ├── output.h        Prototypes affichage et CSV
│   ├── matrice.h       Prototypes allocMat, libMat, initMat
│   ├── simulateur.h    Type ordonnanceur_t (pointeur de fonction)
│   └── scheduler.h     Prototypes scheduler
├── data/
│   ├── test.txt        2 processus avec cycles CPU+E/S
│   ├── test1.txt       3 processus mixtes CPU+E/S
│   └── test2.txt       3 processus CPU (sans E/S)
├── doc/                Documentation Doxygen générée (make doc)
├── Makefile            Compilation + install + doc (wildcard *.c)
├── Doxyfile            Fichier de configuration pour générer la documentation
└── README.md           Ce fichier est un guide d'utilisation et d'installation
```

### Structures de données principales

**`processus_t`** (process.h) — représente un processus avec ses données et ses indicateurs :

```c
typedef struct {
    int pid;                   // Identifiant
    int temps_arrivee;         // Instant d'arrivée (ms)
    int *bursts;               // Tableau alternant CPU/E/S : bursts[0]=CPU, bursts[1]=E/S...
    int nb_bursts;             // Nombre de bursts (impair=fin CPU, pair=fin E/S)
    int index_burst_courant;   // Position courante dans bursts[]
    int temps_cpu_restant;     // Temps CPU restant pour le burst courant
    int temps_io_restant;      // Temps E/S restant si EN_ATTENTE
    etat_processus_t etat;     // NOUVEAU / PRET / EN_EXECUTION / EN_ATTENTE / TERMINE
    int temps_attente;         // Indicateur : temps d'attente total
    int temps_reponse;         // Indicateur : délai premier accès CPU
    int temps_restitution;     // Indicateur : turnaround time
    int temps_fin_execution;   // Instant de terminaison
} processus_t;
```

**`File`** (queue.h) — liste chaînée avec accès O(1) en tête et en queue :

```c
typedef struct cellule {
    processus_t *elements;
    struct cellule *suivant;
} *Liste;

typedef struct { Liste tete; Liste queue; } File;
```

---

## 10. Ajouter un nouvel algorithme

L'architecture garantit qu'ajouter un algorithme ne modifie aucun fichier existant, sauf `main.c` (3 lignes). Voici la démarche complète pour ajouter **Priority Scheduling** :

### Étape 1 — Créer `include/priority.h`

```c
#ifndef PRIORITY_H
#define PRIORITY_H
#include "process.h"

void run_priority(processus_t *processus, int n,
                  resultats_t *resultats, etat_processus_t **gantt);
#endif
```

### Étape 2 — Créer `src/priority.c`

Copier le squelette de `src/sjrf.c` et modifier uniquement `selectionner_processus()` pour appliquer le critère voulu (priorité, deadline, etc.).

### Étape 3 — Déclarer dans `src/main.c`

```c
#include "priority.h"

// Dans le bloc if/else de dispatch :
} else if (strcmp(algo, "priority") == 0) {
    run_priority(p, n, &r, gantt);
}
```

### Étape 4 — Makefile

**Rien à faire.** Le Makefile compile automatiquement tout fichier `*.c` présent dans `src/`.

> ✅ Résultat : 1 nouveau fichier créé + 3 lignes ajoutées dans `main.c`. Aucun autre fichier touché.

---

## 11. Documentation Doxygen

Tous les fichiers source sont commentés au format **Doxygen** (`@file`, `@brief`, `@param`, `@return`). La documentation HTML se génère via :

```bash
make doc
xdg-open doc/html/index.html
```

Chaque en-tête de fichier indique également le **taux de participation** de chaque auteur :

```c
/**
 * @file fifo.c
 * @brief Algorithme FIFO non-préemptif.
 * Participation : DIALLO (33%), DOSSO (33%), MAREGA (33%)
 */
```

---

## 12. Jeux de test fournis

### `data/test.txt` — 2 processus avec E/S

```
2
0  3  2  2  2  2    # P1 : arrive t=3, CPU 3ms · E/S 2ms · CPU 2ms · E/S 2ms
1  2  2  3  3  2    # P2 : arrive t=2, CPU 2ms · E/S 3ms · CPU 3ms · E/S 2ms
```

Ce jeu valide : le parallélisme des E/S, les arrivées décalées, et les processus terminant sur un cycle E/S.

### `data/test1.txt` — 3 processus mixtes

```
3
0  4  2  3  1  2    # P1 : arrive t=4, 4 bursts CPU+E/S
2  2  3  4  2  1    # P2 : arrive t=2, 4 bursts CPU+E/S
4  6  1  2          # P3 : arrive t=6, 2 bursts CPU+E/S
```

Ce jeu valide : la préemption SJRF et la gestion multi-processus avec E/S longues.

### `data/test2.txt` — 3 processus CPU

```
3
0  5    # P1 : arrive t=0, CPU 5ms uniquement
2  3    # P2 : arrive t=2, CPU 3ms uniquement
4  7    # P3 : arrive t=4, CPU 7ms uniquement
```

Ce jeu valide : les cas simples, l'idle CPU et le Round Robin sans E/S.

### Créer son propre jeu de test

```bash
# Exemple : 3 processus personnalisés
cat > data/mon_test.txt << EOF
3
0  5  3  2  4
2  3  1  5  2  3
5  8
EOF
./simulateur data/mon_test.txt rr 2
```

---

## 13. Répartition du travail

| Membre | Contributions                                                                       | Part |
|---|-------------------------------------------------------------------------------------|---|
| DIALLO Thierno Abasse | Algorithmes SJF et SJRF, structure File (`queue.c`), export CSV (`output.c`)        | 33 % |
| DOSSO Siaka | Algorithme FIFO, structure processus (`process.c`), diagramme de Gantt (`matrice.c`) | 33 % |
| MAREGA Aboubacar | Algorithme RR, point d'entrée (`main.c`), Makefile, jeux de test, documentation     | 33 % |

---

## 14. Références

### Documentation C & outils

- [GNU GCC Manual](https://gcc.gnu.org/onlinedocs/) — Compilateur C
- [GNU Make Manual](https://www.gnu.org/software/make/manual/make.html) — Outil de compilation
- [Doxygen Manual](https://www.doxygen.nl/manual/) — Génération de documentation
- [ISO C99 Standard (N1256)](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf) — Standard du langage

---

*Usage académique — CY Cergy Paris Université 2025-2026*
