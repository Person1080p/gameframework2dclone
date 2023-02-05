#ifndef __GME_ENTITY_H__
#define __GME_ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"

#include "gf2d_sprite.h"


typedef struct Entity_S{
    Bool _inuse;
    Sprite *entity_sprite;
    Vector2D entity_pos;
    float entity_frame;
    Color entity_color;
    // void(*think) (struct Entity_S *self);
}Entity;

/**
 * @brief initializes the entity manager 
 * @param max the maximum number of entities the system will handle at once
 */
void gme_entity_init(Uint32 max);
/**
 * @brief make a new entity
*/
Entity *gme_entity_new();
// free, free all


/// @brief draws a specific entity to the screen
/// @param ent entity to draw
void gme_entity_draw(Entity *ent);

/// @brief draws all entites to the screen
void gme_entity_draw_all();

/**
 * @brief closes the entity manager 
 */
void entity_free();
void gme_entity_close();
#endif
