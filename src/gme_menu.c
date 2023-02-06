#include "gme_menu.h"

static Menu menu = {0};

void gme_menu_init()
{
    menu.button_start = gme_entity_new();
    menu.button_exit = gme_entity_new();
    menu.pos = 0;
    
}

void gme_menu_exit()
{

}