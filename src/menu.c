#include <allegro.h>
#include "menu.h"
#include "affichage.h"
#include <string.h>
#include <stdio.h>

/* Libellés des éléments du menu principal */
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
}

int menu_mettre_a_jour(EtatMenu *em) {

    /* Anti-répétition : on attend que la touche soit relâchée */
    static int touche_bas_relachee  = 1;
    static int touche_haut_relachee = 1;
    static int touche_entree_relachee = 1;

    /* Flèche BAS */
    if (key[KEY_DOWN] && touche_bas_relachee) {
        em->element_selectionne = (em->element_selectionne + 1) % NB_ELEMENTS_MENU;
        touche_bas_relachee = 0;
    }
    if (!key[KEY_DOWN]) touche_bas_relachee = 1;

    /* Flèche HAUT */
    if (key[KEY_UP] && touche_haut_relachee) {
        em->element_selectionne = (em->element_selectionne + NB_ELEMENTS_MENU - 1)
                                   % NB_ELEMENTS_MENU;
        touche_haut_relachee = 0;
    }
    if (!key[KEY_UP]) touche_haut_relachee = 1;

    /* ENTRÉE — validation */
    if (key[KEY_ENTER] && touche_entree_relachee) {
        touche_entree_relachee = 0;
        if (em->element_selectionne == MENU_REGLES) {
            em->afficher_regles = 1; /* active l'écran des règles */
            return -1;               /* pas de changement d'état */
        }
        return em->element_selectionne;
    }

    if (!key[KEY_ENTER]) touche_entree_relachee = 1;

    /* Retour depuis les règles */
    if (em->afficher_regles && key[KEY_ESC]) {
        em->afficher_regles = 0;
    }

    return -1;
}


void menu_saisir_pseudo(EtatMenu *em) {
    clear_keybuf();
    int len = strlen(em->pseudo_saisi);

    while (1) {
        /* Affichage géré dans main.c, ici on lit juste le clavier */
        if (keypressed()) {
            int c = readkey();
            int ascii = c & 0xFF;
            int scan  = (c >> 8) & 0xFF;

            if (scan == KEY_ENTER && len > 0) {
                break; /* Pseudo validé */
            } else if (scan == KEY_BACKSPACE && len > 0) {
                em->pseudo_saisi[--len] = '\0';
            } else if (ascii >= 32 && ascii < 127 && len < PSEUDO_LEN - 1) {
                em->pseudo_saisi[len++] = (char)ascii;
                em->pseudo_saisi[len]   = '\0';
            }
        }
    }
}


void afficher_menu(BITMAP *tampon, const EtatMenu *em) {

    if (fond_menu)
        stretch_blit(fond_menu, tampon,
                     0, 0, fond_menu->w, fond_menu->h,
                     0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 30));

    /* Titre */
    textout_centre_ex(tampon, font, "SUPER BULLES",
                      SCREEN_W / 2, SCREEN_H / 5,
                      makecol(255, 200, 0), -1);

    /* Éléments du menu */
    int depart_y = SCREEN_H / 2 - (NB_ELEMENTS_MENU * 40) / 2;

    for (int i = 0; i < NB_ELEMENTS_MENU; i++) {
        int couleur;
        if (i == em->element_selectionne)
            couleur = makecol(255, 255, 0);   /* surligné en jaune */
        else
            couleur = makecol(200, 200, 200); /* gris clair */

        /* Flèche sur l'élément sélectionné */
        if (i == em->element_selectionne)
            textout_ex(tampon, font, ">",
                       SCREEN_W / 2 - 120, depart_y + i * 40,
                       makecol(255, 255, 0), -1);

        textout_centre_ex(tampon, font, LIBELLES_MENU[i],
                          SCREEN_W / 2, depart_y + i * 40,
                          couleur, -1);
    }

    /* Indication de navigation */
    textout_centre_ex(tampon, font, "Fleches haut/bas + Entree pour valider",
                      SCREEN_W / 2, SCREEN_H - 40,
                      makecol(120, 120, 120), -1);
}

void afficher_regles(BITMAP *tampon) {
    /* Fond */
    if (fond_menu)
        blit(fond_menu, tampon, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    else
        clear_to_color(tampon, makecol(10, 10, 50));

    /* Calque sombre pour lisibilité */
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    set_trans_blender(0, 0, 0, 180);
    rectfill(tampon, 40, 30, SCREEN_W - 40, SCREEN_H - 30, makecol(0, 0, 0));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

    /* Bordure */
    rect(tampon, 40, 30, SCREEN_W - 40, SCREEN_H - 30, makecol(100, 100, 200));

    /* Titre */
    textout_centre_ex(tampon, font, "=== REGLES DU JEU ===",
                      SCREEN_W / 2, 50, makecol(255, 220, 0), -1);

    /* Règles */
    const char *regles[] = {
        "OBJECTIF",
        "  Eliminez toutes les bulles de chaque niveau sans vous faire toucher.",
        "  Une partie comporte 4 niveaux de difficulte croissante + un Boss final.",
        "",
        "DEPLACEMENT",
        "  Fleche GAUCHE / DROITE : deplacer le personnage horizontalement.",
        "  Le personnage est bloque sur les bords de la zone de jeu.",
        "",
        "TIRS",
        "  ESPACE : tirer un projectile vers le haut.",
        "  Arme de base : un seul tir a la fois, vitesse constante.",
        "  Des armes speciales apparaissent en detruisant certaines bulles :",
        "  tir rapide, tirs multiples, explosions (disponibles niv. 2+).",
        "",
        "BULLES",
        "  Une bulle touchee se divise en 2 bulles plus petites.",
        "  La plus petite taille est detruite directement.",
        "  A partir du niveau 3, certaines bulles lancent des eclairs !",
        "",
        "FIN DE NIVEAU",
        "  Victoire : toutes les bulles eliminees dans le temps imparti.",
        "  Echec    : touche par une bulle, un eclair, ou temps ecoule.",
        "",
        "  Appuyez sur ECHAP pour revenir au menu."
    };

    int nb_lignes = sizeof(regles) / sizeof(regles[0]);
    int y = 80;

    for (int i = 0; i < nb_lignes; i++) {
        /* Titres de section en jaune, texte normal en blanc */
        int est_titre = (regles[i][0] != ' ' && regles[i][0] != '\0');
        int couleur   = est_titre ? makecol(255, 220, 0) : makecol(220, 220, 220);

        if (regles[i][0] == '\0') {
            y += 6;
            continue;
        }

        textout_ex(tampon, font, regles[i], 60, y, couleur, -1);
        y += 18;
    }
}



