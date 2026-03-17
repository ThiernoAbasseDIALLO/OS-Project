# Simulateur d'ordonnancement de processus
**Documentation Technique & Guide d'utilisation**

Groupe B3 | Système d'exploitation | Université · Année 2025–2026

---

## 1. Présentation du projet

Ce projet implémente un simulateur d'ordonnancement de processus en langage C (standard C99). Il permet de comparer le comportement de quatre algorithmes classiques des systèmes d'exploitation en termes de temps d'attente, de réponse et de restitution (turnaround).

Le simulateur charge une liste de processus depuis un fichier texte, exécute la simulation selon l'algorithme choisi, affiche les résultats dans le terminal et les exporte automatiquement en CSV.

---

## 2. Algorithmes implémentés

### 2.1 FIFO — First In, First Out
Algorithme non-préemptif. Le premier processus arrivé dans la file des prêts obtient le CPU en premier. Simple à implémenter mais peut provoquer l'effet convoi (long processus bloquant les courts).

### 2.2 SJF — Shortest Job First
Algorithme non-préemptif. Parmi tous les processus prêts, celui ayant le plus petit burst CPU est élu. Minimise le temps d'attente moyen, mais peut causer une famine pour les processus longs.

### 2.3 SJRF — Shortest Job Remaining First
Version préemptive du SJF. À chaque milliseconde, le processus avec le plus court temps CPU restant prend le CPU — même s'il faut interrompre le processus en cours. La simulation avance unité par unité (1 ms/tour).

### 2.4 RR — Round Robin
Algorithme préemptif à quantum de temps configurable. Chaque processus reçoit le CPU pour une tranche fixe (quantum). S'il n'a pas fini à l'issue du quantum, il est replacé en fin de file. Garanti l'équité entre processus.

---

## 3. Structure du projet

| Fichier / Dossier | Rôle |
|---|---|
| `src/main.c` | Point d'entrée — chargement des processus, dispatch vers l'algo |
| `src/scheduler.c` | Implémentation de FIFO et SJF (fonctions d'ordonnancement) |
| `src/simulateur.c` | Boucle de simulation générique non-préemptive orientée événements |
| `src/sjrf.c` | Simulation SJRF préemptive (avance ms par ms) |
| `src/rr.c` | Simulation Round Robin avec file circulaire FIFO interne |
| `src/process.c` | Initialisation des structures `processus_t` |
| `src/queue.c` | File générique (enfiler/défiler) pour la file des prêts |
| `src/csv.c` | Export des résultats en fichier CSV |
| `include/` | Headers (`.h`) pour chaque module |
| `data/test.txt` | Fichier de processus de test (5 processus avec bursts CPU/IO) |
| `Makefile` | Compilation du projet (`gcc -std=c99`, exclusion `fifo.c` et `sjf.c`) |

---

## 4. Compilation et exécution

### 4.1 Prérequis
- Système Linux / Ubuntu (testé sous Ubuntu 22.04+)
- `build-essential` : `sudo apt install build-essential`
- `make`

### 4.2 Compilation
Depuis la racine du projet :
```bash
make clean
make
```

### 4.3 Utilisation
```bash
./simulateur <fichier> <algo> [quantum]
```

Exemples :
```bash
./simulateur data/test.txt fifo
./simulateur data/test.txt sjf
./simulateur data/test.txt sjrf
./simulateur data/test.txt rr 4
```

---

## 5. Format du fichier de processus

Chaque ligne représente un processus selon le format suivant :
```
PID  Arrivée  CPU1  [IO1  CPU2  IO2  ...]
```

Les lignes commençant par `#` et les lignes vides sont ignorées.

Exemple (`data/test.txt`) :
```
# Format: ID Arrivée CPU1 IO1 CPU2 IO2 ...
1 0 4 3 2
2 1 3 2 3 1
3 2 5 3
4 3 2 1 3
5 5 3 2 2
```

---

## 6. Métriques calculées

| Métrique | Définition |
|---|---|
| Temps d'attente | Cumul du temps passé en file des prêts sans le CPU |
| Temps de réponse | Premier accès CPU − temps d'arrivée |
| Temps de restitution | Temps de fin − temps d'arrivée (turnaround time) |
| Taux d'utilisation CPU | Pourcentage du temps total où le CPU était occupé |

---

## 7. Résultats sur le jeu de test

Jeu de test : 5 processus, fichier `data/test.txt`

| Algorithme | Att. moy. | Rép. moy. | Rest. moy. | CPU % |
|---|---|---|---|---|
| FIFO | 10.20 | 5.20 | 12.60 | 100% |
| SJF | 8.60 | 6.40 | 7.80 | 90% |
| SJRF | 9.40 | 7.60 | 16.00 | 90% |
| RR (q=4) | 13.40 | 4.80 | 21.20 | 100% |

---

## 8. Sources et références

### 8.1 Ouvrages de référence
- *Operating System Concepts*, 10th ed., Wiley, 2018. Chapitres 5 & 6 : CPU Scheduling.
- *Modern Operating Systems*, 4th ed., Pearson, 2014. Chapitre 2 : Processes and Threads.

### 8.2 Références académiques en ligne
- GeeksForGeeks — "CPU Scheduling in Operating Systems" : https://www.geeksforgeeks.org/cpu-scheduling-in-operating-systems/
- Tutorialspoint — "Operating System — Process Scheduling" : https://www.tutorialspoint.com/operating_system/os_process_scheduling.htm
- Wikipedia — "Scheduling (computing)" : https://en.wikipedia.org/wiki/Scheduling_(computing)

### 8.3 Documentation technique
- ISO/IEC 9899:1999 — Standard C99 : https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
- GNU GCC Manual : https://gcc.gnu.org/onlinedocs/
- GNU Make Manual : https://www.gnu.org/software/make/manual/make.html

---

## 9. Répartition du travail

| Membre | Contributions principales | Part |
|---|---|---|
| Membre 1 | Structure `processus_t`, FIFO, SJF, boucle de simulation générique (`simulateur.c`) | 33 % |
| Membre 2 | SJRF préemptif, file générique (`queue.c`), export CSV | 33 % |
| Membre 3 | Round Robin, Makefile, fichier de test, documentation | 33 % |
