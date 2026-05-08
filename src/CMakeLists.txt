cmake_minimum_required(VERSION 3.10)
project(Projet_INFO_SEM2 C)

set(CMAKE_C_STANDARD 99)

add_executable(Projet_INFO_SEM2
        main.c
        affichage.c
        jeu.c
        entrees.c
        menu.c
        sauvegarde.c
        eclair.c
)

# Includes alfont
target_include_directories(Projet_INFO_SEM2 PRIVATE
        ${CMAKE_SOURCE_DIR}/alfont-code-r12-trunk/alfont-code-r12-trunk/include
)

# Lib alfont compilée
target_link_libraries(Projet_INFO_SEM2
        ${CMAKE_SOURCE_DIR}/alfont-code-r12-trunk/alfont-code-r12-trunk/lib/mingw32/libalfont.a
        -lmingw32
        -lalleg44
        -lm
)


