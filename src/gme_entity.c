#include <simple_logger.h>
#include "gme_entity.h"
// #include "gf2d_sprite.c"
// #include "gf2d_sprite.h"
typedef struct{
    Uint32 entity_max;
    Entity * entity_list;
}EntityManager;

static EntityManager entity_manager = {0}; //I dont know what this really does yet, will figure out l8r
void entity_free(Entity *ent)
{
    if (!ent)
    {
        slog("no entity provided");
        return;
    }
    if (ent->entity_sprite)gf2d_sprite_free(ent->entity_sprite);
    memset(ent,0,sizeof(Entity));
}


void gme_entity_close(){
    // gme_entity_clear_all();
    int i;
    for (i = 0; i < entity_manager.entity_max;i++){
        if (!entity_manager.entity_list[i]._inuse)continue;
        entity_free(&entity_manager.entity_list[i]);
    }
    // entity_manager.entity_list = NULL;
    // entity_manager.max_entity = 0;
    slog("entity system closed");
}
void gme_entity_init(Uint32 max){//PROBABLY WRONG, CHECK l8R
    if (max <= 0)
    {
        slog("cannot intialize entity system: zero entities specified!");
        return;
    }
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),max);
    if (!entity_manager.entity_list)
    {
        slog("failed to initialize entity system!");
        return;
    }
    entity_manager.entity_max = max;
    atexit(gme_entity_close);
    slog("entity system initialized");
}
Entity *gme_entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (entity_manager.entity_list[i]._inuse)continue;
        entity_manager.entity_list[i]._inuse = 1;
        return &entity_manager.entity_list[i];
    }
    return NULL;
}

void gme_entity_draw(Entity *ent)
{
    if (!ent)return;
    if (ent->entity_sprite)
    {
        gf2d_sprite_draw(
            ent->entity_sprite,
            ent->entity_pos,
            NULL,
            NULL,// &ent->entity_drawOffset,
            NULL,// &ent->entity_rotation,
            NULL,
            NULL,
            (Uint32)ent->entity_frame);
    }
    // gf2d_draw_pixel(ent->entity_pos,GFC_COLOR_YELLOW);
    // gf2d_draw_circle(ent->entity_pos,10,GFC_COLOR_YELLOW);
}
void gme_entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (!entity_manager.entity_list[i]._inuse)continue;
        gme_entity_draw(&entity_manager.entity_list[i]);
    }
}
/*eol@eof*/
