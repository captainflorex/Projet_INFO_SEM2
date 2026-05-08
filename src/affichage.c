#include <allegro.h>
#include <alfont.h>
#include "affichage.h"
#include "entites.h"
#include "eclair.h"
#include "menu.h"
#include "jeu.h"
#include <stdio.h>

BITMAP *fond_jeu  = NULL;
BITMAP *fond_menu = NULL;

BITMAP *sprite_bulle_grande    = NULL;
BITMAP *sprite_bulle_moyenne   = NULL;
BITMAP *sprite_bulle_petite    = NULL;
BITMAP *sprite_bulle_minuscule = NULL;

/* Sprites joueur */
static BITMAP *sprite_idle      = NULL;
static BITMAP *sprite_tir       = NULL;
static BITMAP *sprite_marche_d1 = NULL;
static BITMAP *sprite_marche_d2 = NULL;
static BITMAP *sprite_marche_g1 = NULL;
static BITMAP *sprite_marche_g2 = NULL;

/* BOSS */
static BITMAP *sprite_boss[3] = {NULL, NULL, NULL};
static int     anim_boss      = 0;
static int     timer_anim_boss = 0;

static BITMAP *sprite_bonus_explosion = NULL;
static BITMAP *sprite_bonus_triple = NULL;
static BITMAP *sprite_bonus_rapide = NULL;

/* État animation joueur */
static int compteur_anim = 0;
static int duree_tir = 0;


static void appliquer_masque(BITMAP *bmp) {
    if (!bmp) return;
    int magenta = makecol(255, 0, 255);
    int masque  = bitmap_mask_color(bmp);
    for (int y = 0; y < bmp->h; y++)
        for (int x = 0; x < bmp->w; x++)
            if (getpixel(bmp, x, y) == magenta)
                putpixel(bmp, x, y, masque);
}


void affichage_init(void) {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, LARGEUR_FENETRE, HAUTEUR_FENETRE, 0, 0);
    set_window_title("Super Bulles");
}

void affichage_charger_ressources(void) {
    /* Fond jeu */
    fond_jeu = load_bitmap("ressources/fonds/fond_jeu.bmp", NULL);
    if (fond_jeu && (fond_jeu->w != LARGEUR_FENETRE || fond_jeu->h != HAUTEUR_ZONE)) {
        BITMAP *redim = create_bitmap(LARGEUR_FENETRE, HAUTEUR_ZONE);
        stretch_blit(fond_jeu, redim, 0, 0, fond_jeu->w, fond_jeu->h,
                     0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE);
        destroy_bitmap(fond_jeu);
        fond_jeu = redim;
    }
    if (!fond_jeu) {
        fond_jeu = create_bitmap(LARGEUR_FENETRE, HAUTEUR_ZONE);
        clear_to_color(fond_jeu, makecol(20, 20, 80));
    }

    /* Fond menu */
    fond_menu = create_bitmap(LARGEUR_FENETRE, HAUTEUR_FENETRE);
    stretch_blit(fond_jeu, fond_menu, 0, 0, fond_jeu->w, fond_jeu->h,
                 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 150);
    rectfill(fond_menu, 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    /* Sprites bulles */
    sprite_bulle_grande    = load_bitmap("ressources/sprites/bulle_grande.bmp",    NULL);
    sprite_bulle_moyenne   = load_bitmap("ressources/sprites/bulle_moyenne.bmp",   NULL);
    sprite_bulle_petite    = load_bitmap("ressources/sprites/bulle_petite.bmp",    NULL);
    sprite_bulle_minuscule = load_bitmap("ressources/sprites/bulle_minuscule.bmp", NULL);

    appliquer_masque(sprite_bulle_grande);
    appliquer_masque(sprite_bulle_moyenne);
    appliquer_masque(sprite_bulle_petite);
    appliquer_masque(sprite_bulle_minuscule);

    /* Sprites joueur */
    sprite_idle      = load_bitmap("ressources/sprites/joueur_idle.bmp",NULL);
    sprite_tir       = load_bitmap("ressources/sprites/joueur_tir.bmp",NULL);
    sprite_marche_d1 = load_bitmap("ressources/sprites/joueur_marche_d1.bmp",NULL);
    sprite_marche_d2 = load_bitmap("ressources/sprites/joueur_marche_d2.bmp",NULL);
    sprite_marche_g1 = load_bitmap("ressources/sprites/joueur_marche_g1.bmp",NULL);
    sprite_marche_g2 = load_bitmap("ressources/sprites/joueur_marche_g2.bmp",NULL);

    /* BOSS */
    sprite_boss[0] = load_bitmap("ressources/sprites/boss1.bmp",NULL);
    sprite_boss[1] = load_bitmap("ressources/sprites/boss2.bmp",NULL);
    sprite_boss[2] = load_bitmap("ressources/sprites/boss3.bmp",NULL);

    appliquer_masque(sprite_boss[0]);
    appliquer_masque(sprite_boss[1]);
    appliquer_masque(sprite_boss[2]);

    sprite_bonus_explosion = load_bitmap("ressources/sprites/bonus_explosion.bmp",NULL);
    sprite_bonus_triple = load_bitmap("ressources/sprites/bonus_triple.bmp",NULL);
    sprite_bonus_rapide = load_bitmap("ressources/sprites/bonus_rapide.bmp",NULL);

    appliquer_masque(sprite_bonus_rapide);
    appliquer_masque(sprite_bonus_triple);
    appliquer_masque(sprite_bonus_explosion);

    appliquer_masque(sprite_idle);
    appliquer_masque(sprite_tir);
    appliquer_masque(sprite_marche_d1);
    appliquer_masque(sprite_marche_d2);
    appliquer_masque(sprite_marche_g1);
    appliquer_masque(sprite_marche_g2);
}

void affichage_liberer_ressources(void) {
    if (fond_jeu)  { destroy_bitmap(fond_jeu);  fond_jeu  = NULL; }
    if (fond_menu) { destroy_bitmap(fond_menu); fond_menu = NULL; }

    if (sprite_bulle_grande) { destroy_bitmap(sprite_bulle_grande);   sprite_bulle_grande = NULL; }
    if (sprite_bulle_moyenne)  { destroy_bitmap(sprite_bulle_moyenne);  sprite_bulle_moyenne = NULL; }
    if (sprite_bulle_petite) { destroy_bitmap(sprite_bulle_petite);   sprite_bulle_petite  = NULL; }
    if (sprite_bulle_minuscule) { destroy_bitmap(sprite_bulle_minuscule); sprite_bulle_minuscule = NULL; }

    if (sprite_idle)  { destroy_bitmap(sprite_idle); sprite_idle = NULL; }
    if (sprite_tir)   { destroy_bitmap(sprite_tir); sprite_tir = NULL; }
    if (sprite_marche_d1) { destroy_bitmap(sprite_marche_d1); sprite_marche_d1 = NULL; }
    if (sprite_marche_d2) { destroy_bitmap(sprite_marche_d2); sprite_marche_d2 = NULL; }
    if (sprite_marche_g1) { destroy_bitmap(sprite_marche_g1); sprite_marche_g1 = NULL; }
    if (sprite_marche_g2) { destroy_bitmap(sprite_marche_g2); sprite_marche_g2 = NULL; }

    for (int i = 0; i < 3; i++)
        if (sprite_boss[i]) { destroy_bitmap(sprite_boss[i]); sprite_boss[i] = NULL; }

    if (sprite_bonus_explosion) { destroy_bitmap(sprite_bonus_explosion); sprite_bonus_explosion = NULL; }
    if (sprite_bonus_triple) { destroy_bitmap(sprite_bonus_triple); sprite_bonus_triple = NULL; }
    if (sprite_bonus_rapide) { destroy_bitmap(sprite_bonus_rapide); sprite_bonus_rapide = NULL; }
}

/* Appelée depuis main.c quand le joueur tire */
void joueur_signaler_tir(void) {
    duree_tir = 10; /* affiche le sprite tir pendant 10 tics */
}


void afficher_joueur(BITMAP *tampon, const Joueur *j) {
    if (!j->vivant) return;

    BITMAP *sprite = NULL;

    /* Priorité 1 : tir */
    if (duree_tir > 0 && sprite_tir) {
        sprite = sprite_tir;
        duree_tir--;
    }
    /* Priorité 2 : marche droite */
    else if (j->vx > 0) {
        compteur_anim++;
        if (compteur_anim >= 16) compteur_anim = 0;
        sprite = (compteur_anim < 8) ? sprite_marche_d1 : sprite_marche_d2;
    }
    /* Priorité 3 : marche gauche */
    else if (j->vx < 0) {
        compteur_anim++;
        if (compteur_anim >= 16) compteur_anim = 0;
        sprite = (compteur_anim < 8) ? sprite_marche_g1 : sprite_marche_g2;
    }
    /* Priorité 4 : idle */
    else {
        compteur_anim = 0;
        sprite = sprite_idle;
    }

    /* Fallback rectangle si sprite manquant */
    if (!sprite) {
        rectfill(tampon, (int)j->x - 12, (int)j->y - 20,
                         (int)j->x + 12, (int)j->y + 20, makecol(0, 200, 255));
        rect(tampon,     (int)j->x - 12, (int)j->y - 20,
                         (int)j->x + 12, (int)j->y + 20, makecol(255, 255, 255));
        return;
    }

    draw_sprite(tampon, sprite,
                (int)j->x - sprite->w / 2,
                (int)j->y - sprite->h / 2);
}

void afficher_bulle(BITMAP *tampon, const Bulle *b) {
    if (!b->active) return;

    BITMAP *sprite = NULL;
    switch (b->taille) {
        case BULLE_GRANDE:    sprite = sprite_bulle_grande;    break;
        case BULLE_MOYENNE:   sprite = sprite_bulle_moyenne;   break;
        case BULLE_PETITE:    sprite = sprite_bulle_petite;    break;
        case BULLE_MINUSCULE: sprite = sprite_bulle_minuscule; break;
    }

    if (sprite) {
        int taille = (int)(rayon_bulle(b->taille) * 2);
        BITMAP *redim = create_bitmap(taille, taille);
        clear_to_color(redim, bitmap_mask_color(redim));
        stretch_sprite(redim, sprite, 0, 0, taille, taille);
        draw_sprite(tampon, redim,
                    (int)b->x - taille / 2,
                    (int)b->y - taille / 2);
        destroy_bitmap(redim);
    } else {
        int couleurs[4] = {
            makecol(220, 50,  50),
            makecol(230, 140, 20),
            makecol(50,  180, 50),
            makecol(50,  130, 230)
        };
        circlefill(tampon, (int)b->x, (int)b->y,
                   (int)rayon_bulle(b->taille), couleurs[b->taille]);
        circle(tampon, (int)b->x, (int)b->y,
               (int)rayon_bulle(b->taille), makecol(255, 255, 255));
    }
}

void afficher_hud(BITMAP *tampon, const EtatJeu *ej) {
    int y0 = HAUTEUR_ZONE; /* 540 */

    /* Fond dégradé sombre */
    for (int i = 0; i < HAUTEUR_INFO; i++) {
        int val = 15 + i / 3;
        hline(tampon, 0, y0 + i, LARGEUR_FENETRE, makecol(val, val, val + 30));
    }

    /* Ligne de séparation cyan */
    hline(tampon, 0, y0,     LARGEUR_FENETRE, makecol(0,  200, 255));
    hline(tampon, 0, y0 + 1, LARGEUR_FENETRE, makecol(0,  100, 150));

    /* === PSEUDO (gauche) === */
    textprintf_ex(tampon, font, 15, y0 + 10,
                  makecol(150, 150, 150), -1, "JOUEUR");
    textprintf_ex(tampon, font, 15, y0 + 24,
                  makecol(255, 255, 255), -1, "%s", ej->joueur.pseudo);

    /* Séparateur vertical */
    vline(tampon, 160, y0 + 8, y0 + HAUTEUR_INFO - 8, makecol(0, 100, 150));

    /* === SCORE (centre-gauche) === */
    textprintf_ex(tampon, font, 175, y0 + 10,
                  makecol(150, 150, 150), -1, "SCORE");
    textprintf_ex(tampon, font, 175, y0 + 24,
                  makecol(255, 220, 0), -1, "%06d", ej->joueur.score);

    /* Séparateur vertical */
    vline(tampon, 320, y0 + 8, y0 + HAUTEUR_INFO - 8, makecol(0, 100, 150));

    /* === NIVEAU (centre) === */
    textprintf_ex(tampon, font, 335, y0 + 10, makecol(150, 150, 150), -1, "NIVEAU");
    textprintf_ex(tampon, font, 335, y0 + 24, makecol(0, 220, 255), -1, "%d / 4", ej->niveau);

    /* Séparateur vertical */
    vline(tampon, 460, y0 + 8, y0 + HAUTEUR_INFO - 8, makecol(0, 100, 150));

    /* === TEMPS (droite) avec barre de progression === */
    textprintf_ex(tampon, font, 475, y0 + 10,makecol(150, 150, 150), -1, "TEMPS");

    /* Couleur qui vire au rouge quand il reste peu de temps */
    int col_temps;
    if  (ej->temps_restant > 30.0f) col_temps = makecol(0,   255, 100);
    else if (ej->temps_restant > 10.0f) col_temps = makecol(255, 180,   0);
    else  col_temps = makecol(255,  50,  50);

    textprintf_ex(tampon, font, 475, y0 + 24, col_temps, -1, "%.0fs", ej->temps_restant);

    /* Barre de temps */
    int barre_x = 540;
    int barre_largeur = 240;
    int barre_h = 10;
    int barre_y = y0 + 25;
    float ratio = ej->temps_restant / 60.0f;
    if (ratio < 0) ratio = 0;
    if (ratio > 1) ratio = 1;

    /* Fond barre */
    rectfill(tampon, barre_x, barre_y,barre_x + barre_largeur, barre_y + barre_h,makecol(20, 20, 40));
    rect(tampon, barre_x, barre_y,barre_x + barre_largeur, barre_y + barre_h,makecol(0, 80, 120));

    /* Remplissage barre */
    if (ratio > 0)
        rectfill(tampon, barre_x + 1, barre_y + 1,
                 barre_x + 1 + (int)((barre_largeur - 2) * ratio),
                 barre_y + barre_h - 1, col_temps);

    /* Arme active */
    if (ej->joueur.arme != ARME_BASIQUE && ej->timer_arme_speciale > 0) {
        const char *nom_arme = "";
        switch (ej->joueur.arme) {
            case ARME_TIR_RAPIDE: nom_arme = "TIR RAPIDE"; break;
            case ARME_TIR_TRIPLE: nom_arme = "TIR TRIPLE"; break;
            case ARME_EXPLOSION:  nom_arme = "EXPLOSION";  break;
            default: break;
        }
        textprintf_ex(tampon, font, LARGEUR_FENETRE - 200, y0 + 10,makecol(0, 255, 100), -1, "%s  %.0fs", nom_arme, ej->timer_arme_speciale);
    }

}

void afficher_boss(BITMAP *tampon, const Boss *boss) {
    if (!boss->active) return;

    int x = (int)boss->x;
    int y = (int)boss->y;

    /* Animation : change de frame toutes les 8 tics */
    timer_anim_boss++;
    if (timer_anim_boss >= 8) {
        timer_anim_boss = 0;
        anim_boss = (anim_boss + 1) % 3;
    }

    /* Sprite ou fallback cercle */
    if (sprite_boss[anim_boss]) {
        draw_sprite(tampon, sprite_boss[anim_boss],
                    x - sprite_boss[anim_boss]->w / 2,
                    y - sprite_boss[anim_boss]->h / 2);
    } else {
        circlefill(tampon, x, y, 40, makecol(180,   0, 180));
        circle    (tampon, x, y, 40, makecol(255, 100, 255));
    }

    /* Barre de vie (conservée) */
    float ratio   = (float)boss->points_vie / (float)boss->points_vie_max;
    int   barre_w = 120;
    int   barre_x = x - barre_w / 2;
    int   barre_y = y - 70;

    rectfill(tampon, barre_x, barre_y,
             barre_x + barre_w, barre_y + 10, makecol(40, 0, 0));
    rectfill(tampon, barre_x + 1, barre_y + 1,barre_x + 1 + (int)((barre_w - 2) * ratio),barre_y + 9,
             ratio > 0.5f ? makecol(0, 220, 0) :
             ratio > 0.25f ? makecol(255, 180, 0) :
                             makecol(255, 0, 0));
    rect(tampon, barre_x, barre_y,
         barre_x + barre_w, barre_y + 10, makecol(255, 255, 255));
}

void afficher_projectile(BITMAP *tampon, const Projectile *p) {
    if (!p->active) return;

    int x = (int)p->x;
    int y = (int)p->y;

    /* Traînée */
    circlefill(tampon, x, y + 6, 2, makecol(0,  80, 120));
    circlefill(tampon, x, y + 3, 3, makecol(0, 150, 200));

    /* Boule principale */
    circlefill(tampon, x, y, 4, makecol(0,  220, 255));
    circlefill(tampon, x, y, 2, makecol(200, 255, 255));

    /* Halo */
    circle(tampon, x, y, 5, makecol(0, 100, 180));
}



void afficher_jeu(BITMAP *tampon, const EtatJeu *ej) {
    if (fond_jeu)
        blit(fond_jeu, tampon, 0, 0, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE);
    else
        rectfill(tampon, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE, makecol(10, 10, 30));

    for (int i = 0; i < MAX_BULLES; i++)
        afficher_bulle(tampon, &ej->bulles[i]);
    for (int i = 0; i < MAX_PROJECTILES; i++)
        afficher_projectile(tampon, &ej->projectiles[i]);

    /* ===== ECLAIRS ===== */
    if (ej->niveau >= 3)
        dessiner_eclairs(tampon, &ej->eclairs);

    /* Bonus */
    for (int i = 0; i < MAX_BONUS; i++) {
        if (!ej->bonus[i].active) continue;
        int bx = (int)ej->bonus[i].x;
        int by = (int)ej->bonus[i].y;

        /* Clignote dans les 3 dernières secondes */
        if (ej->bonus[i].timer < 3.0f && (int)(ej->bonus[i].timer * 6) % 2 == 0)
            continue;   /* saute l'affichage une frame sur deux */

        if (ej->bonus[i].type == ARME_TIR_RAPIDE - 1 && sprite_bonus_rapide) {
            draw_sprite(tampon, sprite_bonus_rapide,
                        bx - sprite_bonus_rapide->w / 2,
                        by - sprite_bonus_rapide->h / 2);
        } else if (ej->bonus[i].type == ARME_TIR_TRIPLE - 1 && sprite_bonus_triple) {
            draw_sprite(tampon, sprite_bonus_triple,
                        bx - sprite_bonus_triple->w / 2,
                        by - sprite_bonus_triple->h / 2);
        } else if (ej->bonus[i].type == ARME_EXPLOSION - 1 && sprite_bonus_explosion) {
            draw_sprite(tampon, sprite_bonus_explosion,
                        bx - sprite_bonus_explosion->w / 2,
                        by - sprite_bonus_explosion->h / 2);
        } else {
            /* Fallback cercle */
            circlefill(tampon, bx, by, 10, makecol(255, 255, 255));
            circle(tampon,     bx, by, 10, makecol(200, 200, 200));
        }

        /* Flash explosion */
        if (ej->explosion_timer > 0) {
            float ratio = ej->explosion_timer / DUREE_EXPLOSION;
            int rayon = (int)(120 * (1.0f - ratio));
            int alpha = (int)(200 * ratio);
            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
            set_trans_blender(255, 100, 0, alpha);
            circlefill(tampon, (int)ej->explosion_x, (int)ej->explosion_y,rayon, makecol(255, 100, 0));
            drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
            circle(tampon, (int)ej->explosion_x, (int)ej->explosion_y,rayon, makecol(255, 220, 0));
        }
    }

    afficher_joueur(tampon, &ej->joueur);
    if (ej->boss.active)
        afficher_boss(tampon, &ej->boss);
    afficher_hud(tampon, ej);
}


void afficher_pause(BITMAP *tampon, const EtatJeu *ej) {
    /* Overlay sombre semi-transparent */
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 160);
    rectfill(tampon, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    /* Panneau central */
    int px = LARGEUR_FENETRE / 2 - 150;
    int py = HAUTEUR_ZONE    / 2 - 80;
    rectfill(tampon, px,py,+300, py+160, makecol(10,  10,  40));
    rect (tampon, px,py,px+300, py+160, makecol(0,  200, 255));
    rect (tampon, px+2,py+2,px+298, py+158, makecol(0,  80,  120));

    /* Titre PAUSE */
    textout_centre_ex(tampon, font, "PAUSE",LARGEUR_FENETRE / 2, py + 20,makecol(0, 220, 255), -1);

    /* Ligne décorative */
    hline(tampon, px + 20, py + 40, px + 280, makecol(0, 100, 150));

    /* Instructions */
    textout_centre_ex(tampon, font, "P  -  Reprendre",
                      LARGEUR_FENETRE / 2, py + 60,
                      makecol(255, 255, 255), -1);
    textout_centre_ex(tampon, font, "ECHAP  -  Menu principal",
                      LARGEUR_FENETRE / 2, py + 85,
                      makecol(200, 200, 200), -1);

    /* Ligne décorative bas */
    hline(tampon, px + 20, py + 110, px + 280, makecol(0, 100, 150));

    /* Score actuel */
    char buf[32];
    sprintf(buf, "Score : %06d", ej->joueur.score);
    textout_centre_ex(tampon, font, buf,LARGEUR_FENETRE / 2, py + 128,makecol(255, 220, 0), -1);

}

void afficher_decompte(BITMAP *tampon, int compte) {
    /* Overlay sombre */
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 120);
    rectfill(tampon, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int cx = LARGEUR_FENETRE / 2;
    int cy = HAUTEUR_ZONE    / 2;

    /* Couleur selon le chiffre */
    int couleur;
    if      (compte == 3) couleur = makecol(255,  60,  60);
    else if (compte == 2) couleur = makecol(255, 180,   0);
    else                  couleur = makecol(50,  255,  80);

    /* Police externe déclarée dans menu.c — on la recharge ici */
    ALFONT_FONT *police_decompte = alfont_load_font("ressources/police.ttf");
    if (police_decompte) {
        alfont_set_font_size(police_decompte, 180);

        char buf[4];
        sprintf(buf, "%d", compte);

        /* Ombre portée */
        alfont_textout_centre_aa_ex(tampon, police_decompte, buf,
                                    cx + 5, cy - 95, makecol(0, 0, 0), -1);
        /* Halo coloré décalé */
        alfont_textout_centre_aa_ex(tampon, police_decompte, buf,
                                    cx + 3, cy - 93,
                                    makecol(getr(couleur)/3, getg(couleur)/3, getb(couleur)/3), -1);
        /* Chiffre principal */
        alfont_textout_centre_aa_ex(tampon, police_decompte, buf,
                                    cx, cy - 90, couleur, -1);
        /* Reflet blanc léger */
        alfont_set_font_size(police_decompte, 180);
        alfont_textout_centre_aa_ex(tampon, police_decompte, buf,
                                    cx, cy - 92, makecol(
                                        getr(couleur) + (255 - getr(couleur)) / 2,
                                        getg(couleur) + (255 - getg(couleur)) / 2,
                                        getb(couleur) + (255 - getb(couleur)) / 2), -1);

        /* Message en dessous */
        alfont_set_font_size(police_decompte, 28);
        const char *msg = (compte == 1) ? "C'EST PARTI !" : "PRET ?";
        /* Ombre */
        alfont_textout_centre_aa_ex(tampon, police_decompte, msg,
                                    cx + 2, cy + 102, makecol(0, 0, 0), -1);
        /* Texte */
        alfont_textout_centre_aa_ex(tampon, police_decompte, msg,
                                    cx, cy + 100, makecol(255, 255, 255), -1);

        alfont_destroy_font(police_decompte);
    } else {
        /* Fallback texte basique */
        char buf[4];
        sprintf(buf, "%d", compte);
        textout_centre_ex(tampon, font, buf, cx, cy - 20, couleur, -1);
        const char *msg = (compte == 1) ? "C'EST PARTI !" : "PRET ?";
        textout_centre_ex(tampon, font, msg, cx, cy + 30, makecol(255, 255, 255), -1);
    }
}




void afficher_victoire(BITMAP *tampon) {
    if (fond_jeu)
        blit(fond_jeu, tampon, 0, 0, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE);
    else
        clear_to_color(tampon, makecol(0, 50, 0));
    textout_centre_ex(tampon, font, "VICTOIRE !",
                      SCREEN_W/2, SCREEN_H/2-20, makecol(255, 255, 0), -1);
    textout_centre_ex(tampon, font, "Appuyez sur Entree",
                      SCREEN_W/2, SCREEN_H/2+20, makecol(255, 255, 255), -1);
}

void afficher_defaite(BITMAP *tampon) {
    if (fond_jeu)
        blit(fond_jeu, tampon, 0, 0, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE);
    else
        clear_to_color(tampon, makecol(50, 0, 0));
    textout_centre_ex(tampon, font, "GAME OVER",
                      SCREEN_W/2, SCREEN_H/2-20, makecol(255, 0, 0), -1);
    textout_centre_ex(tampon, font, "Appuyez sur Entree",
                      SCREEN_W/2, SCREEN_H/2+20, makecol(255, 255, 255), -1);
}

void afficher_saisie_pseudo(BITMAP *tampon, const EtatMenu *em) {
    if (fond_menu)
        blit(fond_menu, tampon, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 50));

    int px = SCREEN_W / 2 - 200;
    int py = SCREEN_H / 2 - 80;
    rectfill(tampon, px, py, px + 400, py + 160, makecol(20, 20, 60));
    rect(tampon,     px, py, px + 400, py + 160, makecol(100, 100, 255));

    textprintf_centre_ex(tampon, font, SCREEN_W / 2, py + 20,
                         makecol(255, 220, 0), -1, "ENTREZ VOTRE PSEUDO");

    rectfill(tampon, px + 20, py + 60, px + 380, py + 90, makecol(10, 10, 40));
    rect(tampon,     px + 20, py + 60, px + 380, py + 90, makecol(150, 150, 255));

    char affichage[PSEUDO_LEN + 2];
    sprintf(affichage, "%s_", em->pseudo_saisi);
    textprintf_ex(tampon, font, px + 30, py + 68,
                  makecol(255, 255, 255), -1, "%s", affichage);

    textprintf_centre_ex(tampon, font, SCREEN_W / 2, py + 120,
                         makecol(180, 180, 180), -1, "Appuyez sur ENTREE pour valider");
}

void afficher_ecran_sauvegarde(BITMAP *tampon, int choix, const EtatJeu *ej) {
    /* Overlay sombre */
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 160);
    rectfill(tampon, 0, 0, LARGEUR_FENETRE, HAUTEUR_ZONE, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    int px = LARGEUR_FENETRE / 2 - 180;
    int py = HAUTEUR_ZONE    / 2 - 90;

    rectfill(tampon, px,   py,   px+360, py+180, makecol(10,  10,  40));
    rect    (tampon, px,   py,   px+360, py+180, makecol(0,  200, 255));
    rect    (tampon, px+2, py+2, px+358, py+178, makecol(0,   80, 120));

    textout_centre_ex(tampon, font, "SAUVEGARDER ?",
                      LARGEUR_FENETRE / 2, py + 20, makecol(0, 220, 255), -1);
    hline(tampon, px + 20, py + 42, px + 340, makecol(0, 100, 150));

    /* Option OUI */
    int col_oui = (choix == 0) ? makecol(0, 255, 100) : makecol(180, 180, 180);
    if (choix == 0)
        rectfill(tampon, px+30, py+60, px+330, py+85, makecol(0, 40, 20));
    rect(tampon, px+30, py+60, px+330, py+85, col_oui);
    textout_centre_ex(tampon, font, "OUI  -  Sauvegarder et continuer",
                      LARGEUR_FENETRE / 2, py + 68, col_oui, -1);

    /* Option NON */
    int col_non = (choix == 1) ? makecol(255, 80, 80) : makecol(180, 180, 180);
    if (choix == 1)
        rectfill(tampon, px+30, py+100, px+330, py+125, makecol(40, 0, 0));
    rect(tampon, px+30, py+100, px+330, py+125, col_non);
    textout_centre_ex(tampon, font, "NON  -  Continuer sans sauvegarder",
                      LARGEUR_FENETRE / 2, py + 108, col_non, -1);

    /* Info score/niveau */
    hline(tampon, px + 20, py + 138, px + 340, makecol(0, 100, 150));
    textprintf_centre_ex(tampon, font, LARGEUR_FENETRE / 2, py + 152,
                         makecol(255, 220, 0), -1,
                         "Niveau %d  |  Score %06d",
                         ej->niveau, ej->joueur.score);
}

void afficher_liste_sauvegardes(BITMAP *tampon,
                                 const char noms[][PSEUDO_LEN],
                                 int nb, int choix) {
    if (fond_menu)
        blit(fond_menu, tampon, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 50));

    int cx = LARGEUR_FENETRE / 2;
    int py = 150;

    textout_centre_ex(tampon, font, "CHOISIR UNE SAUVEGARDE",
                      cx, py, makecol(0, 220, 255), -1);
    hline(tampon, cx - 200, py + 20, cx + 200, makecol(0, 100, 150));

    for (int i = 0; i < nb; i++) {
        int col = (i == choix) ? makecol(255, 220, 0) : makecol(200, 200, 200);
        if (i == choix)
            rectfill(tampon, cx - 150, py + 40 + i*30 - 2,
                             cx + 150, py + 40 + i*30 + 18, makecol(0, 40, 80));
        textout_centre_ex(tampon, font, noms[i], cx, py + 40 + i * 30, col, -1);
    }

    textout_centre_ex(tampon, font, "HAUT/BAS pour naviguer  ENTREE pour choisir",
                      cx, py + 40 + nb * 30 + 20, makecol(150, 150, 150), -1);
    textout_centre_ex(tampon, font, "ECHAP pour revenir",
                      cx, py + 40 + nb * 30 + 40, makecol(150, 150, 150), -1);
}
