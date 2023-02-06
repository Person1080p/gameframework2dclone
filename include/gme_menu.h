#ifndef __GME_MENU_H__
#define __GME_MENU_H__

#include "gfc_vector.h"
#include "gme_entity.h"

typedef struct Menu_S{
    Entity *button_start;
    Entity *button_exit;
    int pos;
}Menu;

/// @brief creates all entites and renders the game start menu
void gme_menu_init();

/// @brief exits the menu
void gme_menu_exit();


#endif