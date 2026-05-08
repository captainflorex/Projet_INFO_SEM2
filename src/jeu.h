#ifndef JEU_H
#define JEU_H

#include "entites.h"
#include "eclair.h"
#define DUREE_EXPLOSION 0.6f

void jeu_init_niveau(EtatJeu *ej, int niveau);
void jeu_mettre_a_jour(EtatJeu *ej, float dt);
void bulle_deplacer(Bulle *b, float dt, float largeur_zone, float hauteur_zone);
void bulle_diviser(EtatJeu *ej, int indice);
void jeu_verifier_collisions(EtatJeu *ej);
void joueur_tirer(EtatJeu *ej);
void boss_deplacer(Boss *b, float dt, float largeur_zone);
int  jeu_niveau_gagne(const EtatJeu *ej);
int  jeu_niveau_perdu(const EtatJeu *ej);
int  verifier_collision_cercle_rect(float cx, float cy, float r,
    float rx, float ry, float larg, float haut);

#endif
