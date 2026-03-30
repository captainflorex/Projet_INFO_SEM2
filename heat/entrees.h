#ifndef ENTREES_H
#define ENTREES_H

typedef struct {
    int aller_gauche;
    int aller_droite;
    int tirer;
    int pause_appuye;
    int pause;
    int quitter;
} EtatEntrees;

void entrees_lire(EtatEntrees *e);

#endif
