# Super Bulles - Projet INFO S2

Jeu 2D en C avec Allegro 4, inspire de Pang.
Le joueur se deplace horizontalement, tire sur des bulles qui se divisent, et doit finir les niveaux avant la fin du temps.

## Presentation

- Langage: C99
- Bibliotheque graphique: Allegro 4 (`allegro.h`)
- Resolution: 800x600
- Moteur de jeu: boucle a 60 FPS avec timer Allegro
- Etats de l'application:
  - menu principal
  - saisie pseudo
  - en jeu
  - pause
  - victoire / defaite

## Fonctionnalites

- Menu principal:
  - Nouvelle partie
  - Reprendre une partie
  - Regles du jeu
  - Quitter
- 4 niveaux de difficulte croissante
- Systeme de bulles qui se divisent en tailles plus petites
- HUD: pseudo, score, niveau, temps restant
- Ecran de pause et decompte de debut de niveau
- Sauvegarde/chargement simple par pseudo dans `sauvegardes.txt`

## Controles

- Fleche gauche / droite: deplacement
- Espace: tirer
- P: pause / reprise
- Echap:
  - quitter la partie en cours vers le menu
  - revenir depuis l'ecran des regles
- Entree:
  - valider un choix dans le menu
  - valider le pseudo
  - revenir au menu apres victoire/defaite

## Arborescence utile

- `src/`: fichiers source C
- `heat/`: headers
- `ressources/`: assets (fond et sprites)
- `html/` et `latex/`: documentation generee (Doxygen)
- `cmake-build-debug/`: artefacts de build CMake

## Prerequis

Sur Windows (MinGW recommande):

- GCC
- CMake (optionnel)
- Allegro 4 developpement (bibliotheques + headers)

Exemple de bibliotheques a l'edition de liens:

- `-lmingw32`
- `-lalleg44`

## Compilation rapide (GCC)

Depuis la racine du projet (`Projet_INFO_SEM2`):

```bash
gcc -Wall -Wextra -g3 \
  src/main.c src/affichage.c src/jeu.c src/entrees.c src/menu.c src/sauvegarde.c \
  -Iheat -o SuperBulles.exe -lmingw32 -lalleg44
```

Execution:

```bash
./SuperBulles.exe
```

## Compilation avec CMake

Le projet contient un `CMakeLists.txt`.
Si votre environnement Allegro 4 est correctement configure:

```bash
cmake -S . -B build
cmake --build build
```

Puis lancez l'executable genere dans le dossier `build/`.

## Sauvegardes

- Fichier utilise: `sauvegardes.txt` (a la racine d'execution)
- Format d'une ligne: `pseudo niveau`
- Le chargement reprend le dernier niveau trouve pour un pseudo

## Ressources et remarques

- Le fond principal est present dans `ressources/fonds/fond_jeu.bmp`.
- Plusieurs sprites attendus dans `ressources/sprites/` peuvent etre absents.
  - Le code gere des fallback graphiques (formes colorees) si un sprite manque.
- Une partie du chargement de fond dans `src/affichage.c` utilise un chemin absolu local.
  - Si le fichier n'est pas trouve, un fond de secours est cree automatiquement.

## Documentation

- Config Doxygen: `Doxyfile`
- Sorties deja presentes: `html/` et `latex/`

## Auteurs

Floriant CHAFFRAY
Tom PETITJEAN
Remi MATTEODA
Candice OSWALD

