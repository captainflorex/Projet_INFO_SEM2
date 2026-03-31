#include <allegro.h>
#include <alfont.h>
#include "menu.h"
#include "affichage.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static ALFONT_FONT *police_titre  = NULL;
static ALFONT_FONT *police_menu   = NULL;
static ALFONT_FONT *police_petite = NULL;
static ALFONT_FONT *police_regles = NULL;
static int compteur_anim = 0;

static const char *LIBELLES_MENU[NB_ELEMENTS_MENU] = {
    "Nouvelle partie",
    "Reprendre une partie",
    "Regles du jeu",
    "Quitter"
};

void menu_init(EtatMenu *em) {
    em->element_selectionne = 0;
    em->saisie_en_cours     = 0;
    em->afficher_regles     = 0;
    memset(em->pseudo_saisi, 0, PSEUDO_LEN);

    police_titre  = alfont_load_font("ressources/police.ttf");
    police_menu   = alfont_load_font("ressources/police.ttf");
    police_petite = alfont_load_font("ressources/police.ttf");
    police_regles = alfont_load_font("ressources/police.ttf");

    if (police_titre)  alfont_set_font_size(police_titre,  42);
    if (police_menu)   alfont_set_font_size(police_menu,   14);
    if (police_petite) alfont_set_font_size(police_petite, 12);
    if (police_regles) alfont_set_font_size(police_regles, 11);
}

int menu_mettre_a_jour(EtatMenu *em) {
    compteur_anim++;

    static int touche_bas_relachee    = 1;
    static int touche_haut_relachee   = 1;
    static int touche_entree_relachee = 1;

    if (key[KEY_DOWN] && touche_bas_relachee) {
        em->element_selectionne = (em->element_selectionne + 1) % NB_ELEMENTS_MENU;
        touche_bas_relachee = 0;
    }
    if (!key[KEY_DOWN]) touche_bas_relachee = 1;

    if (key[KEY_UP] && touche_haut_relachee) {
        em->element_selectionne = (em->element_selectionne + NB_ELEMENTS_MENU - 1) % NB_ELEMENTS_MENU;
        touche_haut_relachee = 0;
    }
    if (!key[KEY_UP]) touche_haut_relachee = 1;

    if (key[KEY_ENTER] && touche_entree_relachee) {
        touche_entree_relachee = 0;
        if (em->element_selectionne == MENU_REGLES) {
            em->afficher_regles = 1;
            return -1;
        }
        return em->element_selectionne;
    }
    if (!key[KEY_ENTER]) touche_entree_relachee = 1;

    if (em->afficher_regles && key[KEY_ESC])
        em->afficher_regles = 0;

    return -1;
}

void menu_saisir_pseudo(EtatMenu *em) {
    clear_keybuf();
    int len = strlen(em->pseudo_saisi);
    while (1) {
        if (keypressed()) {
            int c     = readkey();
            int ascii = c & 0xFF;
            int scan  = (c >> 8) & 0xFF;
            if (scan == KEY_ENTER && len > 0) break;
            else if (scan == KEY_BACKSPACE && len > 0)
                em->pseudo_saisi[--len] = '\0';
            else if (ascii >= 32 && ascii < 127 && len < PSEUDO_LEN - 1) {
                em->pseudo_saisi[len++] = (char)ascii;
                em->pseudo_saisi[len]   = '\0';
            }
        }
    }
}

void afficher_menu(BITMAP *tampon, const EtatMenu *em) {

    if (fond_menu)
        stretch_blit(fond_menu, tampon, 0, 0, fond_menu->w, fond_menu->h, 0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 30));

    /* Scanlines */
    for (int y = 0; y < SCREEN_H; y += 4) {
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        set_trans_blender(0, 0, 0, 50);
        hline(tampon, 0, y, SCREEN_W, makecol(0, 0, 0));
        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    }

    int cx = SCREEN_W / 2;

    /* === CADRE DU TITRE === */
    int tx = cx - 260, ty = 45;
    int tw = 520,      th = 110;

    /* Halo */
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 180, 255, 30);
    rectfill(tampon, tx - 10, ty - 10, tx + tw + 10, ty + th + 10, makecol(0, 180, 255));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    /* Bordures dorées */
    rect(tampon, tx - 5, ty - 5, tx + tw + 5, ty + th + 5, makecol(255, 180, 0));
    rect(tampon, tx - 4, ty - 4, tx + tw + 4, ty + th + 4, makecol(255, 220, 80));
    rect(tampon, tx - 3, ty - 3, tx + tw + 3, ty + th + 3, makecol(255, 180, 0));

    /* Fond dégradé */
    for (int i = 0; i < th; i++) {
        int r = 5  + (i * 10 / th);
        int g = 5  + (i * 5  / th);
        int b = 20 + (i * 30 / th);
        hline(tampon, tx, ty + i, tx + tw, makecol(r, g, b));
    }

    /* Bordure interne cyan */
    rect(tampon, tx + 5, ty + 5, tx + tw - 5, ty + th - 5, makecol(0, 220, 255));
    rect(tampon, tx + 6, ty + 6, tx + tw - 6, ty + th - 6, makecol(0, 150, 200));

    /* === TITRE avec pulse === */
    if (police_titre) {
        int pulse = (int)(sin(compteur_anim * 0.05) * 3);
        alfont_set_font_size(police_titre, 32 + pulse);
        alfont_textout_centre_aa_ex(tampon, police_titre, "SUPER BULLES",
                                    cx + 3, ty + 38, makecol(0, 40, 60), -1);
        alfont_textout_centre_aa_ex(tampon, police_titre, "SUPER BULLES",
                                    cx + 1, ty + 36, makecol(0, 80, 120), -1);
        alfont_textout_centre_aa_ex(tampon, police_titre, "SUPER BULLES",
                                    cx, ty + 35, makecol(0, 230, 255), -1);
    } else {
        textout_centre_ex(tampon, font, "SUPER BULLES", cx, ty + 35, makecol(0, 220, 255), -1);
    }

    /* Ligne déco */
    hline(tampon, tx + 15, ty + th - 14, tx + tw - 15, makecol(255, 210, 0));
    hline(tampon, tx + 25, ty + th - 12, tx + tw - 25, makecol(255, 160, 0));

    /* === INSERT COIN clignotant === */
    if ((compteur_anim / 30) % 2 == 0) {
        if (police_petite)
            alfont_textout_centre_aa_ex(tampon, police_petite, ">> INSERT COIN <<",
                                        cx, ty + th + 18, makecol(255, 255, 0), -1);
        else
            textout_centre_ex(tampon, font, ">> INSERT COIN <<",
                              cx, ty + th + 18, makecol(255, 255, 0), -1);
    }

    /* === ELEMENTS DU MENU === */
    int depart_y = SCREEN_H / 2 - (NB_ELEMENTS_MENU * 40) / 2 + 30;

    for (int i = 0; i < NB_ELEMENTS_MENU; i++) {
        int y_elem = depart_y + i * 40;

        if (i == em->element_selectionne) {
            /* Halo sélection */
            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
            set_trans_blender(255, 220, 0, 25);
            rectfill(tampon, cx - 225, y_elem - 10, cx + 225, y_elem + 26, makecol(255, 220, 0));
            drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

            /* Cadre */
            rectfill(tampon, cx - 220, y_elem - 8, cx + 220, y_elem + 24, makecol(15, 15, 50));
            rect(tampon,     cx - 220, y_elem - 8, cx + 220, y_elem + 24, makecol(255, 220, 0));

            /* Flèches */
            int dx = (compteur_anim / 10) % 2;
            if (police_menu) {
                alfont_textout_aa_ex(tampon, police_menu, ">", cx - 210 + dx, y_elem, makecol(255, 255, 0), -1);
                alfont_textout_aa_ex(tampon, police_menu, "<", cx + 195 - dx, y_elem, makecol(255, 255, 0), -1);
            }

            /* Texte sélectionné */
            if (police_menu) {
                alfont_textout_centre_aa_ex(tampon, police_menu, LIBELLES_MENU[i],
                                            cx + 1, y_elem + 1, makecol(100, 80, 0), -1);
                alfont_textout_centre_aa_ex(tampon, police_menu, LIBELLES_MENU[i],
                                            cx, y_elem, makecol(255, 240, 50), -1);
            } else {
                textout_centre_ex(tampon, font, LIBELLES_MENU[i], cx, y_elem, makecol(255, 240, 50), -1);
            }
        } else {
            if (police_menu) {
                alfont_textout_centre_aa_ex(tampon, police_menu, LIBELLES_MENU[i],
                                            cx + 1, y_elem + 1, makecol(20, 20, 50), -1);
                alfont_textout_centre_aa_ex(tampon, police_menu, LIBELLES_MENU[i],
                                            cx, y_elem, makecol(160, 170, 210), -1);
            } else {
                textout_centre_ex(tampon, font, LIBELLES_MENU[i], cx, y_elem, makecol(160, 170, 210), -1);
            }
        }
    }

    /* === BAS === */
    hline(tampon, 40, SCREEN_H - 70, SCREEN_W - 40, makecol(50, 50, 100));

    if (police_petite) {
        alfont_textout_centre_aa_ex(tampon, police_petite, "HI-SCORE : 999999",
                                    cx, SCREEN_H - 58, makecol(255, 80, 80), -1);
        alfont_textout_centre_aa_ex(tampon, police_petite, "- 2025 SUPER BULLES -",
                                    cx, SCREEN_H - 38, makecol(100, 100, 180), -1);
        alfont_textout_centre_aa_ex(tampon, police_petite, "FLECHES + ENTREE",
                                    cx, SCREEN_H - 18, makecol(80, 80, 100), -1);
    } else {
        textout_centre_ex(tampon, font, "HI-SCORE : 999999",     cx, SCREEN_H - 58, makecol(255, 80, 80), -1);
        textout_centre_ex(tampon, font, "- 2025 SUPER BULLES -", cx, SCREEN_H - 38, makecol(100, 100, 180), -1);
        textout_centre_ex(tampon, font, "FLECHES + ENTREE",      cx, SCREEN_H - 18, makecol(80, 80, 100), -1);
    }
}

void afficher_regles(BITMAP *tampon) {
    if (fond_menu)
        blit(fond_menu, tampon, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 50));

    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 190);
    rectfill(tampon, 40, 30, SCREEN_W - 40, SCREEN_H - 30, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    rect(tampon, 40, 30, SCREEN_W - 40, SCREEN_H - 30, makecol(255, 200, 0));
    rect(tampon, 42, 32, SCREEN_W - 42, SCREEN_H - 32, makecol(100, 100, 200));

    /* Titre des règles avec police_regles (taille 15) */
    if (police_regles) {
        alfont_set_font_size(police_regles, 15);
        alfont_textout_centre_aa_ex(tampon, police_regles, "REGLES DU JEU",
                                    SCREEN_W / 2 + 2, 47, makecol(80, 60, 0), -1);
        alfont_textout_centre_aa_ex(tampon, police_regles, "REGLES DU JEU",
                                    SCREEN_W / 2, 45, makecol(255, 220, 0), -1);
        alfont_set_font_size(police_regles, 11);
    } else {
        textout_centre_ex(tampon, font, "=== REGLES DU JEU ===", SCREEN_W / 2, 50, makecol(255, 220, 0), -1);
    }

    const char *regles[] = {
        "OBJECTIF",
        "  Eliminez toutes les bulles de chaque niveau sans vous faire ",
        "",
        "  toucher.",
        "",
        "  Une partie comporte 4 niveaux de difficulte croissante ",
        "",
        "  + un Boss final."
        "",
        "DEPLACEMENT",
        "  Fleche GAUCHE/DROITE : deplacer le personnage horizontalement.",
        "",
        "  Le personnage est bloque sur les bords de la zone de jeu.",
        "",
        "TIRS",
        "  ESPACE : tirer un projectile vers le haut.",
        "",
        "  Arme de base : un seul tir a la fois, vitesse constante.",
        "",
        "  Des armes speciales apparaissent en detruisant des bulles :",
        "",
        "  tir rapide, tirs multiples, explosions (disponibles niv. 2+).",
        "",
        "BULLES",
        "  Une bulle touchee se divise en 2 bulles plus petites.",
        "",
        "  La plus petite taille est detruite directement.",
        "",
        "  A partir du niveau 3, certaines bulles lancent des eclairs !",
        "",
        "FIN DE NIVEAU",
        "  Victoire : toutes les bulles eliminees dans le temps imparti.",
        "",
        "  Echec    : touche par une bulle, un eclair, ou temps ecoule.",
        "",
        "  Appuyez sur ECHAP pour revenir au menu."
    };

    int nb_lignes = sizeof(regles) / sizeof(regles[0]);
    int y = 75;

    for (int i = 0; i < nb_lignes; i++) {
        if (regles[i][0] == '\0') { y += 6; continue; }
        int est_titre = (regles[i][0] != ' ');
        int couleur   = est_titre ? makecol(255, 220, 0) : makecol(200, 210, 230);
        if (police_regles)
            alfont_textout_aa_ex(tampon, police_regles, regles[i], 60, y, couleur, -1);
        else
            textout_ex(tampon, font, regles[i], 60, y, couleur, -1);
        y += est_titre ? 16 : 13;
    }
}
