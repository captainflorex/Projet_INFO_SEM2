#include "eclair.h"
#include <string.h>
#include <allegro.h>

void eclairs_init(ListeEclairs *le) {
    memset(le, 0, sizeof(ListeEclairs));
    le->nb = 0;
}

void eclair_spawner(ListeEclairs *le, float x, float y) {
    if (le->nb >= MAX_ECLAIRS) return;

    for (int i = 0; i < MAX_ECLAIRS; i++) {
        if (!le->liste[i].actif) {
            le->liste[i].x      = x;
            le->liste[i].y      = y;
            le->liste[i].actif  = 1;
            le->liste[i].timer  = ECLAIR_DUREE_VIE;
            if (i >= le->nb) le->nb = i + 1;
            return;
        }
    }
}

void eclairs_mettre_a_jour(ListeEclairs *le) {
    for (int i = 0; i < MAX_ECLAIRS; i++) {
        if (!le->liste[i].actif) continue;

        le->liste[i].y += ECLAIR_VITESSE;
        le->liste[i].timer--;

        /* Désactiver si sorti de l'écran ou timer écoulé */
        if (le->liste[i].y > 1000 || le->liste[i].timer <= 0)
            le->liste[i].actif = 0;
    }
}

int eclairs_collision_personnage(const ListeEclairs *le,
                                  float px, float py,
                                  int plargeur, int phauteur) {
    for (int i = 0; i < MAX_ECLAIRS; i++) {
        if (!le->liste[i].actif) continue;

        float ex = le->liste[i].x;
        float ey = le->liste[i].y;

        /* AABB simple : l'éclair est une ligne fine (ECLAIR_LARGEUR px) */
        if (ex + ECLAIR_LARGEUR > px &&
            ex                  < px + plargeur &&
            ey                  > py &&
            ey                  < py + phauteur) {
            return 1;
            }
    }
    return 0;
}

void dessiner_eclairs(BITMAP *tampon, const ListeEclairs *le) {
    for (int i = 0; i < MAX_ECLAIRS; i++) {
        if (!le->liste[i].actif) continue;

        int x = (int)le->liste[i].x;
        int y = (int)le->liste[i].y;

        vline(tampon, x,     y - 20, y, makecol(255, 255, 100));
        vline(tampon, x + 1, y - 20, y, makecol(255, 255, 255));
        vline(tampon, x + 2, y - 20, y, makecol(255, 255, 100));

        putpixel(tampon, x,     y + 1, makecol(200, 200, 255));
        putpixel(tampon, x + 1, y + 1, makecol(255, 255, 255));

        circlefill(tampon, x + 1, y - 20, 4, makecol(100, 100, 255));
        circlefill(tampon, x + 1, y - 20, 2, makecol(200, 200, 255));
    }
}
