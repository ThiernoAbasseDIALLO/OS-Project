#include <stdio.h>
#include "csv.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 512
/*
 * Export des résultats de simulation au format CSV.
 * Compatible tableur (Excel, LibreOffice).
 */

void exporter_csv(const char *nom_fichier, resultats_t r) {
    int desc;
    off_t pos;
}
