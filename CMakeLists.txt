cmake_minimum_required(VERSION 3.10)
project(Projet_INFO_SEM2 C)

set(CMAKE_C_STANDARD 99)

# Sources
add_executable(Projet_INFO_SEM2
        src/main.c
        src/affichage.c
        src/jeu.c
        src/entrees.c
        src/menu.c
        src/sauvegarde.c
        src/eclair.c
        header/eclair.h
        header/affichage.h
        header/alfont.h
        header/alfontdll.h
        header/entites.h
        header/entrees.h
        header/jeu.h
        header/menu.h
        header/sauvegarde.h
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
