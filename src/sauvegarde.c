#include "sauvegarde.h"
#include <stdio.h>
#include <string.h>

#define NOGDI // Source Claude: car sinon bug avec allegro
#define WIN32_LEAN_AND_MEAN // Source Claude: car sinon bug avec allegro
#include <windows.h>

/* Nom de fichier unique par pseudo */
static void nom_fichier(const char *pseudo, char *buf, int buf_size) {
    snprintf(buf, buf_size, "sauvegarde_%s.txt", pseudo);
}

void sauvegarde_ecrire(const EtatJeu *ej) {
    char fichier[64];
    nom_fichier(ej->joueur.pseudo, fichier, sizeof(fichier));
    FILE *f = fopen(fichier, "w");
    if (!f) return;
    fprintf(f, "%s\n%d\n%d\n", ej->joueur.pseudo, ej->joueur.score, ej->niveau);
    fclose(f);
}

int sauvegarde_lire(EtatJeu *ej) {
    char fichier[64];
    nom_fichier(ej->joueur.pseudo, fichier, sizeof(fichier));
    FILE *f = fopen(fichier, "r");
    if (!f) return 0;
    char pseudo[PSEUDO_LEN];
    int  score, niveau;
    if (fscanf(f, "%49s\n%d\n%d\n", pseudo, &score, &niveau) != 3) {
        fclose(f); return 0;
    }
    strncpy(ej->joueur.pseudo, pseudo, PSEUDO_LEN);
    ej->joueur.score = score;
    ej->niveau       = niveau;
    fclose(f);
    return 1;
}

int sauvegarde_existe(const char *pseudo) {
    char fichier[64];
    nom_fichier(pseudo, fichier, sizeof(fichier));
    FILE *f = fopen(fichier, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int sauvegarde_lister(char noms[][PSEUDO_LEN], int max) {
    WIN32_FIND_DATA fd;
    HANDLE h;
    int nb = 0;

    h = FindFirstFileA("sauvegarde_*.txt", &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;

    do {
        if (nb >= max) break;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        char *debut = fd.cFileName + 11; /* len("sauvegarde_") == 11 */
        char *fin   = strrchr(fd.cFileName, '.');
        if (!fin || fin <= debut) continue;

        int len = (int)(fin - debut);
        if (len <= 0 || len >= PSEUDO_LEN) continue;

        strncpy(noms[nb], debut, len);
        noms[nb][len] = '\0';
        nb++;

    } while (FindNextFileA(h, &fd) != 0);

    FindClose(h);
    return nb;
}