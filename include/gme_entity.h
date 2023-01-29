#ifndef __GME_ENTITY_H__
#define __GME_ENTITY_H__

#include "gf2d_sprite.h"

typedef struct Entity_S{
    Sprite *entity_sprite;
    Vector2D entity_pos;
    float entity_frame;
    Color entity_color;
}Entity;

/**
 * @brief initializes the entity manager 
 * @param max the maximum number of entities the system will handle at once
 */
void gme_entity_init(Uint32 max);

/**
 * @brief closes the entity manager 
 */
void gme_entity_close();
#endif
