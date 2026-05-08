#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include "entites.h"

#define NB_ELEMENTS_MENU 4

typedef enum {
 MENU_NOUVELLE_PARTIE = 0,
 MENU_REPRENDRE_PARTIE,
 MENU_REGLES,
 MENU_QUITTER
} ElementMenu;

typedef struct {
 int  element_selectionne;
 char pseudo_saisi[PSEUDO_LEN];
 int  saisie_en_cours;
 int  afficher_regles;
} EtatMenu;

void menu_init(EtatMenu *em);
int  menu_mettre_a_jour(EtatMenu *em);
void menu_saisir_pseudo(EtatMenu *em);
void afficher_menu(BITMAP *tampon, const EtatMenu *em);
void afficher_regles(BITMAP *tampon);

#endif
