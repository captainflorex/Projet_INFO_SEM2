#include "jeu.h"
#include "entites.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

float rayon_bulle(TailleBulle t) {
    switch(t) {
        case BULLE_GRANDE:    return 40.0f;
        case BULLE_MOYENNE:   return 25.0f;
        case BULLE_PETITE:    return 15.0f;
        case BULLE_MINUSCULE: return  8.0f;
    }
    return 8.0f;
}

void jeu_init_niveau(EtatJeu *ej, int niveau) {
    memset(ej, 0, sizeof(EtatJeu));
    ej->niveau        = niveau;
    ej->temps_restant = 60.0f;
    ej->decompte      = 3;

    ej->joueur.x      = LARGEUR_FENETRE / 2.0f;
    ej->joueur.y      = HAUTEUR_ZONE - 30.0f;
    ej->joueur.vivant = 1;
    ej->joueur.arme   = ARME_BASIQUE;

    /* Une bulle de départ par niveau (augmente avec le niveau) */
    for (int i = 0; i < niveau && i < MAX_BULLES; i++) {
        ej->bulles[i].active = 1;
        ej->bulles[i].taille = BULLE_GRANDE;
        ej->bulles[i].x      = 100.0f + i * 150.0f;
        ej->bulles[i].y      = 100.0f;
        ej->bulles[i].vx     = 80.0f + niveau * 10.0f;
        ej->bulles[i].vy     = 0.0f;
        ej->nb_bulles++;
    }
}

void jeu_mettre_a_jour(EtatJeu *ej, float dt) {

    /* Décompte de début de niveau */
    if (ej->decompte > 0) {
        ej->decompte_timer += dt;
        if (ej->decompte_timer >= 1.0f) {
            ej->decompte_timer = 0.0f;
            ej->decompte--;
        }
        return; /* on bloque le jeu pendant le décompte */
    }

    /* Déplacement joueur */
    ej->joueur.x += ej->joueur.vx * dt;
    if (ej->joueur.x < 12)                   ej->joueur.x = 12;
    if (ej->joueur.x > LARGEUR_FENETRE - 12) ej->joueur.x = LARGEUR_FENETRE - 12;


    /* Déplacement bulles */
    for (int i = 0; i < MAX_BULLES; i++)
        bulle_deplacer(&ej->bulles[i], dt, LARGEUR_FENETRE, HAUTEUR_ZONE);

    /* Déplacement projectiles */
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!ej->projectiles[i].active) continue;
        ej->projectiles[i].y += ej->projectiles[i].vy * dt;
        if (ej->projectiles[i].y < 0) ej->projectiles[i].active = 0;
    }

    /* Boss */
    if (ej->boss.active) boss_deplacer(&ej->boss, dt, LARGEUR_FENETRE);

    /* Collisions */
    jeu_verifier_collisions(ej);

    /* Temps */
    ej->temps_restant -= dt;
}

void bulle_deplacer(Bulle *b, float dt, float largeur_zone, float hauteur_zone) {
    if (!b->active) return;

    b->vy += 100.0f * dt; /* gravité */
    b->x  += b->vx * dt;
    b->y  += b->vy * dt;

    float r = rayon_bulle(b->taille);

    if (b->x - r < 0)            { b->x = r;               b->vx =  fabsf(b->vx); }
    if (b->x + r > largeur_zone) { b->x = largeur_zone - r; b->vx = -fabsf(b->vx); }
    if (b->y + r > hauteur_zone) { b->y = hauteur_zone - r; b->vy = -fabsf(b->vy) * 0.95f; }
    if (b->y - r < 0)            { b->y = r;                b->vy =  fabsf(b->vy); }
}

void bulle_diviser(EtatJeu *ej, int indice) {
    Bulle *b = &ej->bulles[indice];
    if (b->taille == BULLE_MINUSCULE) { b->active = 0; return; }

    TailleBulle prochaine = b->taille + 1;
    float bx = b->x, by = b->y;
    b->active = 0;

    int crees = 0;
    for (int i = 0; i < MAX_BULLES && crees < 2; i++) {
        if (!ej->bulles[i].active) {
            ej->bulles[i].active        = 1;
            ej->bulles[i].taille        = prochaine;
            ej->bulles[i].x             = bx;
            ej->bulles[i].y             = by;
            ej->bulles[i].vx            = (crees == 0 ? -120.0f : 120.0f);
            ej->bulles[i].vy            = -150.0f;
            ej->bulles[i].contient_arme = 0;
            ej->bulles[i].lance_eclairs = 0;
            crees++;
        }
    }
}

void jeu_verifier_collisions(EtatJeu *ej) {
    Joueur *j = &ej->joueur;
    float rj = 16.0f;

    for (int i = 0; i < MAX_BULLES; i++) {
        Bulle *b = &ej->bulles[i];
        if (!b->active) continue;
        float rb = rayon_bulle(b->taille);

        /* Projectile / bulle */
        for (int k = 0; k < MAX_PROJECTILES; k++) {
            Projectile *p = &ej->projectiles[k];
            if (!p->active) continue;
            float dx = p->x - b->x, dy = p->y - b->y;
            if (dx*dx + dy*dy < (rb+4)*(rb+4)) {
                p->active = 0;
                ej->joueur.score += 10 * (4 - b->taille);
                bulle_diviser(ej, i);
                break;
            }
        }

        /* Bulle / joueur */
        if (!b->active) continue;
        float dx = j->x - b->x, dy = j->y - b->y;
        if (dx*dx + dy*dy < (rj+rb)*(rj+rb))
            j->vivant = 0;
    }

    /* Boss / joueur */
    if (ej->boss.active) {
        float dx = j->x - ej->boss.x, dy = j->y - ej->boss.y;
        if (dx*dx + dy*dy < (rj+30)*(rj+30))
            j->vivant = 0;
    }
}

void joueur_tirer(EtatJeu *ej) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!ej->projectiles[i].active) {
            ej->projectiles[i].active = 1;
            ej->projectiles[i].x      = ej->joueur.x;
            ej->projectiles[i].y      = ej->joueur.y - 20.0f;
            ej->projectiles[i].vy     = -400.0f;
            break;
        }
    }
}

void boss_deplacer(Boss *b, float dt, float largeur_zone) {
    if (!b->active) return;
    b->x += b->vx * dt;
    if (b->x < 30 || b->x > largeur_zone - 30) b->vx = -b->vx;
}

int jeu_niveau_gagne(const EtatJeu *ej) {
    for (int i = 0; i < MAX_BULLES; i++)
        if (ej->bulles[i].active) return 0;
    if (ej->boss.active) return 0;
    return 1;
}

int jeu_niveau_perdu(const EtatJeu *ej) {
    return (!ej->joueur.vivant || ej->temps_restant <= 0.0f);
}

int verifier_collision_cercle_rect(float cx, float cy, float r,
                                   float rx, float ry, float larg, float haut) {
    float px = cx < rx ? rx : (cx > rx+larg ? rx+larg : cx);
    float py = cy < ry ? ry : (cy > ry+haut ? ry+haut : cy);
    float dx = cx - px, dy = cy - py;
    return (dx*dx + dy*dy) < (r*r);
}
