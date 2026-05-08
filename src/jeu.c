#include "jeu.h"
#include "eclair.h"
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

    /* Sauvegarde avant reset */
    int score_sauvegarde = (niveau > 1) ? ej->joueur.score : 0;
    char pseudo_sauvegarde[PSEUDO_LEN];
    strncpy(pseudo_sauvegarde, ej->joueur.pseudo, PSEUDO_LEN);

    memset(ej, 0, sizeof(EtatJeu));

    /* Restauration après reset */
    ej->joueur.score = score_sauvegarde;
    strncpy(ej->joueur.pseudo, pseudo_sauvegarde, PSEUDO_LEN);

    eclairs_init(&ej->eclairs);
    ej->niveau        = niveau;
    ej->temps_restant = 60.0f;
    ej->decompte      = 3;

    ej->joueur.x      = LARGEUR_FENETRE / 2.0f;
    ej->joueur.y      = HAUTEUR_ZONE - 30.0f;
    ej->joueur.vivant = 1;
    ej->joueur.arme   = ARME_BASIQUE;

    if (niveau <= 2) {
        /* Niveaux 1 et 2 : bulles visibles dès le départ */
        for (int i = 0; i < niveau && i < MAX_BULLES; i++) {
            ej->bulles[i].active = 1;
            ej->bulles[i].taille = BULLE_GRANDE;
            ej->bulles[i].x      = 100.0f + i * 150.0f;
            ej->bulles[i].y      = 100.0f;
            ej->bulles[i].vx     = 80.0f + niveau * 10.0f;
            ej->bulles[i].vy     = 0.0f;
            ej->nb_bulles++;
        }
    } else if (niveau == 4) {
        /* Niveau 4 : le boss apparaît après le décompte */
        ej->boss.active         = 0;   /* spawne après décompte */
        ej->boss.points_vie     = 10;
        ej->boss.points_vie_max = 10;
        ej->boss.x              = LARGEUR_FENETRE / 2.0f;
        ej->boss.y              = 80.0f;
        ej->boss.vx             = 100.0f;
        ej->boss.timer_tir      = 2.0f;
    } else {
        ej->bulles_en_attente = niveau;
    }
}


void jeu_mettre_a_jour(EtatJeu *ej, float dt) {

    /* Décompte de début de niveau */
    if (ej->decompte > 0) {
        ej->decompte_timer += dt;
        if (ej->decompte_timer >= 1.0f) {
            ej->decompte_timer = 0.0f;
            ej->decompte--;

            if (ej->decompte == 0) {
                /* Spawn bulles en attente (niveaux 3) */
                if (ej->bulles_en_attente > 0) {
                    for (int i = 0; i < ej->bulles_en_attente && i < MAX_BULLES; i++) {
                        ej->bulles[i].active = 1;
                        ej->bulles[i].taille = BULLE_GRANDE;
                        ej->bulles[i].x      = 100.0f + i * 150.0f;
                        ej->bulles[i].y      = 100.0f;
                        ej->bulles[i].vx     = 80.0f + ej->niveau * 10.0f;
                        ej->bulles[i].vy     = 0.0f;
                        ej->nb_bulles++;
                    }
                    ej->bulles_en_attente = 0;
                }

                /* Spawn boss niveau 4 — condition séparée */
                if (ej->niveau == 4 && !ej->boss_a_spawne) {
                    ej->boss.active    = 1;
                    ej->boss_a_spawne  = 1;
                }
            }
        }
        return;
    }

    /* Bonus qui tombent au sol */
    if (ej->niveau >= 2) {
        for (int i = 0; i < MAX_BONUS; i++) {
            if (!ej->bonus[i].active) continue;
            /* Décompte timer */
            ej->bonus[i].timer -= dt;
            if (ej->bonus[i].timer <= 0) {
                ej->bonus[i].active = 0;   /* ← disparaît */
                continue;
            }
            /* Chute */
            ej->bonus[i].y += 80.0f * dt;
            if (ej->bonus[i].y > HAUTEUR_ZONE - 10)
                ej->bonus[i].y = HAUTEUR_ZONE - 10;

            /* Collision joueur / bonus */
            float dx = ej->joueur.x - ej->bonus[i].x;
            float dy = ej->joueur.y - ej->bonus[i].y;
            if (dx*dx + dy*dy < 30*30) {
                ej->joueur.arme           = ej->bonus[i].type + 1; /* +1 car ARME_BASIQUE = 0 */
                ej->timer_arme_speciale   = DUREE_ARME_SPECIALE;
                ej->bonus[i].active        = 0;
            }
        }

        /* Décompte durée arme spéciale */
        if (ej->timer_arme_speciale > 0) {
            ej->timer_arme_speciale -= dt;
            if (ej->timer_arme_speciale <= 0) {
                ej->timer_arme_speciale = 0;
                ej->joueur.arme = ARME_BASIQUE;
            }
        }
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
        ej->projectiles[i].x += ej->projectiles[i].vx * dt;
        ej->projectiles[i].y += ej->projectiles[i].vy * dt;
        if (ej->projectiles[i].y < 0) ej->projectiles[i].active = 0;
    }

    /* Boss */
    if (ej->boss.active) boss_deplacer(&ej->boss, dt, LARGEUR_FENETRE);

    /* Tir du boss */
    if (ej->boss.active) {
        ej->boss.timer_tir -= dt;
        if (ej->boss.timer_tir <= 0) {
            float ratio = (float)ej->boss.points_vie / (float)ej->boss.points_vie_max;
            ej->boss.timer_tir = 0.5f + ratio * 1.5f;
            for (int i = 0; i < MAX_BULLES; i++) {
                if (!ej->bulles[i].active) {
                    ej->bulles[i].active = 1;
                    ej->bulles[i].taille = BULLE_PETITE;
                    ej->bulles[i].x      = ej->boss.x;
                    ej->bulles[i].y      = ej->boss.y + 40.0f;
                    ej->bulles[i].vx     = (rand() % 2 == 0 ? -100.0f : 100.0f);
                    ej->bulles[i].vy     = 80.0f;
                    break;
                }
            }
        }
    }

    /* Collisions */
    jeu_verifier_collisions(ej);

    /* ===== ECLAIRS (niveau 3+) ===== */
    if (ej->niveau >= 3) {
        for (int i = 0; i < MAX_BULLES; i++) {
            if (ej->bulles[i].active && rand() % 200 == 0)
                eclair_spawner(&ej->eclairs, ej->bulles[i].x, ej->bulles[i].y);
        }
        eclairs_mettre_a_jour(&ej->eclairs);

        if (eclairs_collision_personnage(&ej->eclairs,
                                          ej->joueur.x, ej->joueur.y,
                                          24.0f, 32.0f)) {
            ej->joueur.vivant = 0;
                                          }
    }
    /* Timer explosion visuelle */
    if (ej->explosion_timer > 0)
        ej->explosion_timer -= dt;

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

    /* 1 chance sur 4 de faire tomber un bonus (niveau 2+) */
    if (ej->niveau >= 2 && rand() % 4 == 0) {
        for (int i = 0; i < MAX_BONUS; i++) {
            if (!ej->bonus[i].active) {
                ej->bonus[i].active = 1;
                ej->bonus[i].x     = b->x;
                ej->bonus[i].y     = b->y;
                ej->bonus[i].type  = rand() % NB_TYPES_BONUS;
                ej->bonus[i].timer  = 8.0f;
                break;
            }
        }
    }

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

                if (p->explosion) {
                    ej->explosion_x     = p->x;
                    ej->explosion_y     = p->y;
                    ej->explosion_timer = DUREE_EXPLOSION;
                    /* Explose toutes les bulles dans un rayon de 80px */
                    for (int e = 0; e < MAX_BULLES; e++) {
                        if (!ej->bulles[e].active) continue;
                        float ex = ej->bulles[e].x - p->x;
                        float ey = ej->bulles[e].y - p->y;
                        if (ex*ex + ey*ey < 80*80) {
                            ej->joueur.score += 10 * (4 - ej->bulles[e].taille);
                            bulle_diviser(ej, e);
                        }
                    }
                    break;
                } else {
                    bulle_diviser(ej, i);
                  }
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
        if (dx*dx + dy*dy < (rj+40)*(rj+40))
            j->vivant = 0;

        /* Projectile / boss */
        for (int k = 0; k < MAX_PROJECTILES; k++) {
            Projectile *p = &ej->projectiles[k];
            if (!p->active) continue;
            float bx = p->x - ej->boss.x, by = p->y - ej->boss.y;
            if (bx*bx + by*by < 40*40) {
                p->active = 0;
                ej->joueur.score += 50;
                ej->boss.points_vie--;
                if (ej->boss.points_vie <= 0)
                    ej->boss.active = 0;
            }
        }
    }
}

void joueur_tirer(EtatJeu *ej) {
    /* Arme basique : 1 seul tir à la fois */
    if (ej->joueur.arme == ARME_BASIQUE) {
        for (int i = 0; i < MAX_PROJECTILES; i++)
            if (ej->projectiles[i].active) return;
    }

    switch (ej->joueur.arme) {

        case ARME_BASIQUE:
        default:
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!ej->projectiles[i].active) {
                    ej->projectiles[i].active    = 1;
                    ej->projectiles[i].x         = ej->joueur.x;
                    ej->projectiles[i].y         = ej->joueur.y - 20.0f;
                    ej->projectiles[i].vy        = -700.0f;
                    ej->projectiles[i].explosion = 0;
                    break;
                }
            }
            break;

        case ARME_TIR_RAPIDE:
            /* Tir rapide : pas de limite 1 balle */
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!ej->projectiles[i].active) {
                    ej->projectiles[i].active    = 1;
                    ej->projectiles[i].x         = ej->joueur.x;
                    ej->projectiles[i].y         = ej->joueur.y - 20.0f;
                    ej->projectiles[i].vy        = -900.0f;
                    ej->projectiles[i].explosion = 0;
                    break;
                }
            }
            break;

        case ARME_TIR_TRIPLE:
            /* 3 projectiles : gauche, centre, droite */
            { int crees = 0;
              float vx_vals[3] = { -150.0f, 0.0f, 150.0f };
              for (int i = 0; i < MAX_PROJECTILES && crees < 3; i++) {
                  if (!ej->projectiles[i].active) {
                      ej->projectiles[i].active    = 1;
                      ej->projectiles[i].x         = ej->joueur.x;
                      ej->projectiles[i].y         = ej->joueur.y - 20.0f;
                      ej->projectiles[i].vx        = vx_vals[crees];
                      ej->projectiles[i].vy        = -700.0f;
                      ej->projectiles[i].explosion = 0;
                      crees++;
                  }
              }
            }
            break;

        case ARME_EXPLOSION:
            /* 1 seul tir à la fois comme l'arme basique */
                for (int i = 0; i < MAX_PROJECTILES; i++)
                    if (ej->projectiles[i].active) return;   /* ← AJOUTE CE BLOCAGE */

        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!ej->projectiles[i].active) {
                ej->projectiles[i].active    = 1;
                ej->projectiles[i].x         = ej->joueur.x;
                ej->projectiles[i].y         = ej->joueur.y - 20.0f;
                ej->projectiles[i].vx        = 0.0f;
                ej->projectiles[i].vy        = -700.0f;
                ej->projectiles[i].explosion = 1;
                break;
            }
        }
        break;
    }
}

void boss_deplacer(Boss *b, float dt, float largeur_zone) {
    if (!b->active) return;

    float ratio   = (float)b->points_vie / (float)b->points_vie_max;
    float vitesse = b->vx * (1.0f + (1.0f - ratio) * 2.0f);

    b->x += vitesse * dt;
    if (b->x < 40)                { b->x = 40;                b->vx =  fabsf(b->vx); }
    if (b->x > largeur_zone - 40) { b->x = largeur_zone - 40; b->vx = -fabsf(b->vx); }
}

int jeu_niveau_gagne(const EtatJeu *ej) {
    if (ej->decompte > 0) return 0;
    if (ej->bulles_en_attente > 0) return 0;

    /* Niveau 4 : le boss doit avoir spawné ET être mort */
    if (ej->niveau == 4) {
        if (!ej->boss_a_spawne) return 0;   /* ← pas encore apparu */
        if (ej->boss.active)    return 0;   /* ← encore vivant */
        return 1;
    }

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
