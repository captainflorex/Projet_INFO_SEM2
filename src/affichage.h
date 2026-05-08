#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <allegro.h>
#include "entites.h"
#include "menu.h"

extern BITMAP *fond_jeu;
extern BITMAP *fond_menu;

extern BITMAP *sprite_bulle_grande;
extern BITMAP *sprite_bulle_moyenne;
extern BITMAP *sprite_bulle_petite;
extern BITMAP *sprite_bulle_minuscule;

void affichage_init(void);
void affichage_charger_ressources(void);
void affichage_liberer_ressources(void);

void afficher_jeu(BITMAP *tampon, const EtatJeu *ej);
void afficher_pause(BITMAP *tampon, const EtatJeu *ej);
void afficher_hud(BITMAP *tampon, const EtatJeu *ej);

void afficher_bulle(BITMAP *tampon, const Bulle *b);
void afficher_joueur(BITMAP *tampon, const Joueur *j);
void afficher_projectile(BITMAP *tampon, const Projectile *p);
void afficher_boss(BITMAP *tampon, const Boss *boss);
void afficher_decompte(BITMAP *tampon, int compte);
void afficher_victoire(BITMAP *tampon);
void afficher_defaite(BITMAP *tampon);
void afficher_saisie_pseudo(BITMAP *tampon, const EtatMenu *em);

void afficher_ecran_sauvegarde(BITMAP *tampon, int choix, const EtatJeu *ej);
void afficher_liste_sauvegardes(BITMAP *tampon,const char noms[][PSEUDO_LEN],int nb, int choix);

/* Animation joueur */
void joueur_signaler_tir(void);

#endif
