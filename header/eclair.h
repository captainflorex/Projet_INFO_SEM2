#include <allegro.h>

#ifndef ECLAIR_H
#define ECLAIR_H

#define MAX_ECLAIRS 10
#define ECLAIR_LARGEUR 6
#define ECLAIR_VITESSE 5
#define ECLAIR_DUREE_VIE 120  /* frames avant disparition si pas de collision */

typedef struct {
    float x, y;
    int actif;
    int timer;         /* compteur de vie */
} Eclair;

typedef struct {
    Eclair liste[MAX_ECLAIRS];
    int nb;
} ListeEclairs;

/* Initialise la liste */
void eclairs_init(ListeEclairs *le);

/* Fait tomber un éclair depuis la position x,y d'une bulle */
void eclair_spawner(ListeEclairs *le, float x, float y);

/* Met à jour tous les éclairs (déplacement + timer) */
void eclairs_mettre_a_jour(ListeEclairs *le);

/* Retourne 1 si un éclair touche le personnage */
int eclairs_collision_personnage(const ListeEclairs *le,
                                  float px, float py,
                                  int plargeur, int phauteur);

void dessiner_eclairs(BITMAP *tampon, const ListeEclairs *le);

#endif
