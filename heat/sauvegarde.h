#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "entites.h"  /* pour PSEUDO_LEN */

#define FICHIER_SAUVEGARDE "sauvegardes.txt"

/*
 * Sauvegarde le pseudo et le niveau atteint dans le fichier de sauvegarde.
 * Plusieurs parties peuvent être sauvegardées dans le même fichier.
 * Entrée : pseudo -> nom du joueur
 *          niveau -> dernier niveau gagné
 */
void sauvegarder_partie(const char *pseudo, int niveau);

/*
 * Charge la dernière sauvegarde correspondant au pseudo donné.
 * Entrée  : pseudo -> nom du joueur recherché
 * Sortie  : numéro du dernier niveau sauvegardé, ou -1 si non trouvé
 */
int charger_partie(const char *pseudo);

/*
 * Affiche la liste de toutes les parties sauvegardées.
 * Utile pour le menu "Reprendre une partie".
 * Entrée : tampons    -> tableau de chaînes pour stocker les pseudos trouvés
 *          niveaux    -> tableau d'entiers pour stocker les niveaux associés
 *          taille_max -> taille maximale des tableaux
 * Sortie : nombre de sauvegardes trouvées
 */
int lister_sauvegardes(char tampons[][PSEUDO_LEN], int *niveaux, int taille_max);

#endif
