#include <allegro.h>
#include <alfont.h>
#include "affichage.h"
#include "jeu.h"
#include "entrees.h"
#include "menu.h"
#include "sauvegarde.h"

/* Inclure windows.h SANS wingdi.h (qui redéfinit BITMAP) */
#define NOGDI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

volatile int tics = 0;
void compteur(void) { tics++; }
END_OF_FUNCTION(compteur)

typedef enum {
    ETAT_MENU,
    ETAT_SAISIE_PSEUDO,
    ETAT_SAISIE_PSEUDO_REPRISE,
    ETAT_EN_JEU,
    ETAT_PAUSE,
    ETAT_VICTOIRE,
    ETAT_DEFAITE,
    ETAT_QUITTER,
    ETAT_SAUVEGARDE
} EtatApplication;

/* Liste les sauvegardes disponibles, retourne le nombre trouvé */
static int lister_sauvegardes(char noms[][PSEUDO_LEN], int max) {
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile("sauvegarde_*.txt", &fd);
    int nb = 0;
    if (h == INVALID_HANDLE_VALUE) return 0;
    do {
        if (nb >= max) break;
        char *debut = fd.cFileName + strlen("sauvegarde_");
        char *fin   = strrchr(fd.cFileName, '.');
        if (debut && fin && fin > debut) {
            int len = (int)(fin - debut);
            if (len >= PSEUDO_LEN) len = PSEUDO_LEN - 1;
            strncpy(noms[nb], debut, len);
            noms[nb][len] = '\0';
            nb++;
        }
    } while (FindNextFile(h, &fd));
    FindClose(h);
    return nb;
}

int main(void) {
    affichage_init();
    affichage_charger_ressources();
    alfont_init();

    BITMAP *tampon = create_bitmap(LARGEUR_FENETRE, HAUTEUR_FENETRE);
    LOCK_VARIABLE(tics);
    LOCK_FUNCTION(compteur);
    install_int_ex(compteur, BPS_TO_TIMER(60));

    EtatJeu         ej;
    EtatEntrees     entrees;
    EtatMenu        menu_etat;
    EtatApplication etat = ETAT_MENU;
    menu_init(&menu_etat);
    memset(&ej, 0, sizeof(EtatJeu));

    int choix_sauvegarde = 0;

    /* Liste des sauvegardes pour "Reprendre" */
    char  liste_sauvegardes[10][PSEUDO_LEN];
    int   nb_sauvegardes  = 0;
    int   choix_reprise   = 0;

    while (etat != ETAT_QUITTER) {

        while (tics > 0) {
            tics--;
            float dt = 1.0f / 60.0f;
            entrees_lire(&entrees);

            switch (etat) {

                /* ===== MENU ===== */
                case ETAT_MENU: {
                    int action = menu_mettre_a_jour(&menu_etat);
                    if (action == MENU_NOUVELLE_PARTIE) {
                        memset(menu_etat.pseudo_saisi, 0, PSEUDO_LEN);
                        clear_keybuf();
                        etat = ETAT_SAISIE_PSEUDO;
                    } else if (action == MENU_REPRENDRE_PARTIE) {
                        nb_sauvegardes = sauvegarde_lister(liste_sauvegardes, 10);
                        choix_reprise  = 0;
                        /* reset de la garde ENTER pour éviter validation immédiate */
                        clear_keybuf();
                        if (nb_sauvegardes == 0) {
                            /* rien */
                        } else {
                            etat = ETAT_SAISIE_PSEUDO_REPRISE;
                        }

                    } else if (action == MENU_QUITTER) {
                        etat = ETAT_QUITTER;
                    }
                    break;
                }

                /* ===== SAISIE PSEUDO NOUVELLE PARTIE ===== */
                case ETAT_SAISIE_PSEUDO: {
                    if (keypressed()) {
                        int c     = readkey();
                        int ascii = c & 0xFF;
                        int scan  = (c >> 8) & 0xFF;
                        int len   = strlen(menu_etat.pseudo_saisi);

                        if (scan == KEY_ENTER && len > 0) {
                            memset(&ej, 0, sizeof(EtatJeu));
                            jeu_init_niveau(&ej, 1);
                            strcpy(ej.joueur.pseudo, menu_etat.pseudo_saisi);
                            etat = ETAT_EN_JEU;
                        } else if (scan == KEY_BACKSPACE && len > 0) {
                            menu_etat.pseudo_saisi[len - 1] = '\0';
                        } else if (ascii >= 32 && ascii < 127 && len < PSEUDO_LEN - 1) {
                            menu_etat.pseudo_saisi[len]     = (char)ascii;
                            menu_etat.pseudo_saisi[len + 1] = '\0';
                        }
                    }
                    break;
                }

                /* ===== CHOIX SAUVEGARDE A REPRENDRE ===== */
                case ETAT_SAISIE_PSEUDO_REPRISE: {
                    static int entree_relachee = 0;  /* attend que ENTER soit relâché */

                    /* Attend que ENTER soit relâché avant d'accepter une nouvelle pression */
                    if (!key[KEY_ENTER]) entree_relachee = 1;

                    if (key[KEY_UP]   && choix_reprise > 0)                  choix_reprise--;
                    if (key[KEY_DOWN] && choix_reprise < nb_sauvegardes - 1) choix_reprise++;

                    if (key[KEY_ENTER] && entree_relachee) {
                        entree_relachee = 0;
                        strncpy(ej.joueur.pseudo, liste_sauvegardes[choix_reprise], PSEUDO_LEN);
                        sauvegarde_lire(&ej);
                        jeu_init_niveau(&ej, ej.niveau);
                        etat = ETAT_EN_JEU;
                    }
                    if (key[KEY_ESC]) etat = ETAT_MENU;
                    break;
                }

                /* ===== EN JEU ===== */
                case ETAT_EN_JEU: {
                    if (entrees.aller_gauche)      ej.joueur.vx = -200.0f;
                    else if (entrees.aller_droite) ej.joueur.vx =  200.0f;
                    else                           ej.joueur.vx =    0.0f;

                    if (entrees.tirer) {
                        joueur_tirer(&ej);
                        joueur_signaler_tir();
                    }
                    if (entrees.quitter) {
                        clear_keybuf();
                        etat = ETAT_MENU;
                    }
                    if (entrees.pause_appuye)
                        etat = ETAT_PAUSE;

                    jeu_mettre_a_jour(&ej, dt);

                    if (jeu_niveau_gagne(&ej)) {
                        choix_sauvegarde = 0;
                        etat = ETAT_SAUVEGARDE;
                    }
                    if (jeu_niveau_perdu(&ej)) etat = ETAT_DEFAITE;
                    break;
                }

                /* ===== SAUVEGARDE ===== */
                case ETAT_SAUVEGARDE: {
                    if (key[KEY_UP]   || key[KEY_LEFT])  choix_sauvegarde = 0;
                    if (key[KEY_DOWN] || key[KEY_RIGHT]) choix_sauvegarde = 1;

                    if (key[KEY_ENTER]) {
                        ej.niveau++;   /* incrémente AVANT de sauvegarder */

                        if (choix_sauvegarde == 0)
                            sauvegarde_ecrire(&ej);

                        choix_sauvegarde = 0;

                        if (ej.niveau > 4) {
                            etat = ETAT_VICTOIRE;
                        } else {
                            jeu_init_niveau(&ej, ej.niveau);
                            etat = ETAT_EN_JEU;
                        }
                    }
                    break;
                }

                /* ===== PAUSE ===== */
                case ETAT_PAUSE: {
                    if (entrees.pause_appuye) etat = ETAT_EN_JEU;
                    if (entrees.quitter) {
                        clear_keybuf();
                        etat = ETAT_MENU;
                    }
                    break;
                }

                /* ===== VICTOIRE / DEFAITE ===== */
                case ETAT_VICTOIRE:
                case ETAT_DEFAITE: {
                    if (key[KEY_ENTER]) etat = ETAT_MENU;
                    break;
                }

                default: break;
            }
        }

        /* === RENDU === */
        clear_bitmap(tampon);

        switch (etat) {
            case ETAT_MENU:
                if (menu_etat.afficher_regles)
                    afficher_regles(tampon);
                else
                    afficher_menu(tampon, &menu_etat);
                break;

            case ETAT_SAISIE_PSEUDO:
                afficher_saisie_pseudo(tampon, &menu_etat);
                break;

            case ETAT_SAISIE_PSEUDO_REPRISE:
                /* Affiche la liste des sauvegardes */
                afficher_liste_sauvegardes(tampon,
                    (const char (*)[PSEUDO_LEN])liste_sauvegardes,
                    nb_sauvegardes, choix_reprise);
                break;

            case ETAT_EN_JEU:
                afficher_jeu(tampon, &ej);
                if (ej.decompte > 0)
                    afficher_decompte(tampon, ej.decompte);
                break;

            case ETAT_SAUVEGARDE:
                afficher_jeu(tampon, &ej);
                afficher_ecran_sauvegarde(tampon, choix_sauvegarde, &ej);
                break;

            case ETAT_PAUSE:
                afficher_jeu(tampon, &ej);
                afficher_pause(tampon, &ej);
                break;

            case ETAT_VICTOIRE:
                afficher_victoire(tampon);
                break;

            case ETAT_DEFAITE:
                afficher_defaite(tampon);
                break;

            default: break;
        }

        vsync();
        blit(tampon, screen, 0, 0, 0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE);
    }

    affichage_liberer_ressources();
    destroy_bitmap(tampon);
    alfont_exit();
    allegro_exit();
    return 0;
}
END_OF_MAIN()