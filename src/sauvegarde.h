#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "entites.h"

#define FICHIER_SAUVEGARDE "sauvegarde.txt"

void sauvegarde_ecrire(const EtatJeu *ej);
int  sauvegarde_lire(EtatJeu *ej);   /* retourne 1 si succès, 0 sinon */
int sauvegarde_existe(const char *pseudo);
int sauvegarde_lister(char noms[][PSEUDO_LEN], int max);

#endif