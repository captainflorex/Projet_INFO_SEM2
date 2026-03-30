/* entrees.c */
#include "entrees.h"
#include <allegro.h>

void entrees_lire(EtatEntrees *e) {
    static int pause_precedent = 0;

    e->aller_gauche  = key[KEY_LEFT];
    e->aller_droite  = key[KEY_RIGHT];
    e->tirer         = key[KEY_SPACE];
    e->quitter       = key[KEY_ESC];

    int pause_actuel = key[KEY_P];
    e->pause_appuye  = (pause_actuel && !pause_precedent); /* vrai 1 seul tic */
    e->pause         = pause_actuel;
    pause_precedent  = pause_actuel;
}

