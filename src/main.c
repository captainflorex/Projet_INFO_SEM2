#include <allegro.h>
#include <string.h>
#include "affichage.h"
#include "jeu.h"
#include "entrees.h"
#include "menu.h"
#include "sauvegarde.h"

volatile int tics = 0;
void compteur(void) { tics++; }
END_OF_FUNCTION(compteur)

//test

typedef enum {
    ETAT_MENU,
    ETAT_SAISIE_PSEUDO,
    ETAT_EN_JEU,
    ETAT_PAUSE,
    ETAT_VICTOIRE,
    ETAT_DEFAITE,
    ETAT_QUITTER
} EtatApplication;

int main(void) {
    affichage_init();
    affichage_charger_ressources();

    BITMAP *tampon = create_bitmap(LARGEUR_FENETRE, HAUTEUR_FENETRE);
    LOCK_VARIABLE(tics);
    LOCK_FUNCTION(compteur);
    install_int_ex(compteur, BPS_TO_TIMER(60));

    EtatJeu         ej;
    EtatEntrees     entrees;
    EtatMenu        menu_etat;
    EtatApplication etat = ETAT_MENU;
    menu_init(&menu_etat);

    while (etat != ETAT_QUITTER) {

        /* === LOGIQUE === */
        while (tics > 0) {
            tics--;
            float dt = 1.0f / 60.0f;

            entrees_lire(&entrees);

            switch (etat) {

                case ETAT_MENU: {
                    int action = menu_mettre_a_jour(&menu_etat);
                    if (action == MENU_NOUVELLE_PARTIE) {
                        memset(menu_etat.pseudo_saisi, 0, PSEUDO_LEN);
                        etat = ETAT_SAISIE_PSEUDO;
                    } else if (action == MENU_REPRENDRE_PARTIE) {
                        int niveau = charger_partie(menu_etat.pseudo_saisi);
                        if (niveau > 0) {
                            jeu_init_niveau(&ej, niveau);
                            strcpy(ej.joueur.pseudo, menu_etat.pseudo_saisi);
                            etat = ETAT_EN_JEU;
                        }
                    } else if (action == MENU_QUITTER) {
                        etat = ETAT_QUITTER;
                    }
                    break;
                }

                case ETAT_SAISIE_PSEUDO: {
                    if (keypressed()) {
                        int c     = readkey();
                        int ascii = c & 0xFF;
                        int scan  = (c >> 8) & 0xFF;
                        int len   = strlen(menu_etat.pseudo_saisi);

                        if (scan == KEY_ENTER && len > 0) {
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

                case ETAT_EN_JEU: {
                    if (entrees.aller_gauche)       ej.joueur.vx = -200.0f;
                    else if (entrees.aller_droite)  ej.joueur.vx =  200.0f;
                    else                            ej.joueur.vx =    0.0f;

                    if (entrees.tirer) {
                        joueur_tirer(&ej);
                        joueur_signaler_tir();
                    }
                    if (entrees.quitter)      etat = ETAT_MENU;
                    if (entrees.pause_appuye) etat = ETAT_PAUSE;

                    jeu_mettre_a_jour(&ej, dt);

                    if (jeu_niveau_gagne(&ej)) {
                        if (ej.niveau < 4) jeu_init_niveau(&ej, ej.niveau + 1);
                        else               etat = ETAT_VICTOIRE;
                    }
                    if (jeu_niveau_perdu(&ej)) etat = ETAT_DEFAITE;
                    break;
                }

                case ETAT_PAUSE: {
                    if (entrees.pause_appuye) etat = ETAT_EN_JEU; /* ← simplifié */
                    if (entrees.quitter)      etat = ETAT_MENU;
                    break;
                }


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

            case ETAT_EN_JEU:
                afficher_jeu(tampon, &ej);
                if (ej.decompte > 0)
                    afficher_decompte(tampon, ej.decompte);
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
    allegro_exit();
    return 0;
}
END_OF_MAIN()
