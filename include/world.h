#ifndef __WORLD_H__
#define __WORLD_H__

#include "gfc_vector.h"
#include "entity.h"

/**
 * @brief spawn a new player at the given location
 * @param position where to spawn the bug at
 * @return NULL on failure, or a pointer to the new bug
 */
Entity *world_player_new(Vector2D position);


#endif
