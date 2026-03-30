#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

void sauvegarder_partie(const char *pseudo, int niveau) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "a");
    if (!f) return;
    fprintf(f, "%s %d\n", pseudo, niveau);
    fclose(f);
}

int charger_partie(const char *pseudo) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return -1;

    char buf[PSEUDO_LEN];
    int  niveau, trouve = -1;

    while (fscanf(f, "%s %d", buf, &niveau) == 2)
        if (strcmp(buf, pseudo) == 0)
            trouve = niveau;

    fclose(f);
    return trouve; /* retourne le dernier niveau sauvegardé, -1 si introuvable */
}

int lister_sauvegardes(char tampons[][PSEUDO_LEN], int *niveaux, int taille_max) {
    FILE *f = fopen(FICHIER_SAUVEGARDE, "r");
    if (!f) return 0;

    int nb = 0;
    while (nb < taille_max &&
           fscanf(f, "%s %d", tampons[nb], &niveaux[nb]) == 2)
        nb++;

    fclose(f);
    return nb; /* retourne le nombre de sauvegardes trouvées */
}
