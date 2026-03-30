cmake_minimum_required(VERSION 3.10)
project(Projet_INFO_SEM2 C)

set(CMAKE_C_STANDARD 99)

# Sources
add_executable(Projet_INFO_SEM2
        main.c
        affichage.c
        jeu.c
        entrees.c
        menu.c
        sauvegarde.c
)

target_link_libraries(Projet_INFO_SEM2 -lmingw32 -lalleg44)


