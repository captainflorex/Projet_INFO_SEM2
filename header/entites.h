#ifndef ENTITES_H
#define ENTITES_H

#include "eclair.h"

#define LARGEUR_FENETRE  800
#define HAUTEUR_FENETRE  600
#define HAUTEUR_INFO      60
#define HAUTEUR_ZONE     (HAUTEUR_FENETRE - HAUTEUR_INFO)

#define MAX_BULLES      30
#define MAX_PROJECTILES 10
#define MAX_BONUS       5
#define PSEUDO_LEN      32

#define NB_TYPES_BONUS 3
#define DUREE_ARME_SPECIALE 10.0f



typedef enum {
    BULLE_GRANDE = 0,
    BULLE_MOYENNE,
    BULLE_PETITE,
    BULLE_MINUSCULE
} TailleBulle;

typedef enum {
    ARME_BASIQUE = 0,
    ARME_TIR_RAPIDE,
    ARME_TIR_TRIPLE,
    ARME_EXPLOSION
} TypeArme;

typedef struct {
    float x, y;
    float vx;
    int   vivant;
    TypeArme arme;
    float duree_arme;
    int   score;
    char  pseudo[PSEUDO_LEN];
} Joueur;

typedef struct {
    float       x, y;
    float       vx, vy;
    TailleBulle taille;
    int         active;
    int         contient_arme;
    int         lance_eclairs;
} Bulle;

typedef struct {
    float x, y;
    float vx, vy;
    int active;
    int explosion;
} Projectile;

typedef struct {
    float    x, y;
    TypeArme type;
    int active;
    float timer;
} BonusArme;

typedef struct {
    float x, y;
    float vx, vy;
    int   points_vie;
    int   active;
} Boss;

typedef struct {
    Joueur      joueur;
    Bulle       bulles[MAX_BULLES];
    int         nb_bulles;
    Projectile  projectiles[MAX_PROJECTILES];
    BonusArme   bonus[MAX_BONUS];
    Boss        boss;
    int         niveau;
    float       temps_restant;
    int         decompte;
    float decompte_timer;
    ListeEclairs eclairs;
    int bulles_en_attente;   /* nb de bulles à spawner après le décompte */
    float timer_arme_speciale;
    float explosion_x, explosion_y;
    float explosion_timer;   /* > 0 = explosion en cours */
} EtatJeu;


float rayon_bulle(TailleBulle t);

#endif
