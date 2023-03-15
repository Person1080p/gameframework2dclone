#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"

#include "gf2d_sprite.h"
#include "camera.h"

typedef struct Entity_S
{
    Bool    _entity_inuse;
    Sprite *entity_sprite;
    float   entity_frame;
    float   entity_rotation;
    float   entity_speed;
    Vector2D entity_drawOffset;
    
    
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    
    void (*think)(struct Entity_S *self);
    int (*update)(struct Entity_S *self);
    void (*draw)(struct Entity_S *self);
    void (*free_entity)(struct Entity_S *self);//cleanup of custom data if defined
    void *data;
}Entity;

/**
 * @brief initialize the internal manager for the entity system
 * @note this will automatically queue up the close function for program exit
 * @param max this is the maximum number of supported entities at a given time
 */
void gme_entity_manager_init(Uint32 max);

/**
 * @brief alloate an initialize a new entity
 * @return NULL if there are no entities left, an empty entity otherwise
 */
Entity *gme_entity_new();

/**
 * @brief free a previously allocated entity
 * @param ent the entity to free
 */
void gme_entity_free(Entity *ent);

/**
 * @brief free all allocated entities
 */
void gme_entity_free_all();

/**
 * @brief draw all active entities if they have graphics
 */
void gme_entity_draw_all();

/**
 * @brief update all active entities
 */
void gme_entity_update_all();

/**
 * @brief call all the think functions for the entities, if they have one
 */
void gme_entity_think_all();

/// @brief continues animation for entity
/// @param ent entity pointer
/// @param frame_start start pos of animation
/// @param frame_final max value of animation
void gme_entity_animate(Entity *ent, float frame_start, float frame_final);

void gme_entity_think(Entity *self);


#endif
