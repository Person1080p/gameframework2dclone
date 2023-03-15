#include "simple_logger.h"
#include "gf2d_draw.h"
#include "gme_entity.h"

typedef struct
{
    Uint32  entity_max;
    Entity *entity_list;
    SJson  *entity_def;
}EntityManager;

static EntityManager entity_manager = {0};

void gme_entity_manager_close()
{
    gme_entity_free_all();
    if (entity_manager.entity_list)free(entity_manager.entity_list);
    slog("entity system closed");
}

void gme_entity_manager_init(Uint32 max)
{
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
    // entity_manager.entity_def = sj_load("config/entities.def");
    atexit(gme_entity_manager_close);
    slog("entity system initialized");
}

void gme_entity_free_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (!entity_manager.entity_list[i]._entity_inuse)continue;
        gme_entity_free(&entity_manager.entity_list[i]);
    }
}

Entity *gme_entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (entity_manager.entity_list[i]._entity_inuse)continue;
        entity_manager.entity_list[i]._entity_inuse = 1;
        return &entity_manager.entity_list[i];
    }
    return NULL;
}

void gme_entity_free(Entity *ent)
{
    if (!ent)
    {
        return;
    }
    if (ent->entity_sprite)gf2d_sprite_free(ent->entity_sprite);
    memset(ent,0,sizeof(Entity));
}

void gme_entity_draw(Entity *ent)
{
    if (!ent)return;
    if (ent->entity_sprite)
    {
        gf2d_sprite_draw(
            ent->entity_sprite,
            ent->position,
            NULL,
            &ent->entity_drawOffset,
            &ent->entity_rotation,
            NULL,
            NULL,
            (Uint32)ent->entity_frame);
    }
    // gf2d_draw_pixel(ent->position,GFC_COLOR_YELLOW);
    // gf2d_draw_circle(ent->position,10,GFC_COLOR_YELLOW);
}

void gme_entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (!entity_manager.entity_list[i]._entity_inuse)continue;
        gme_entity_draw(&entity_manager.entity_list[i]);
    }
}

void gme_entity_update(Entity *ent)
{
    if (!ent)return;
    if (ent->update)
    {
        if (ent->update(ent))return;// if the update function returns 1, do not do generic update
    }
    ent->entity_frame += 0.1;
    if (ent->entity_frame >= 16)ent->entity_frame = 0;
    vector2d_add(ent->position,ent->position,ent->velocity);
    if(vector2d_magnitude_compare(ent->velocity,0) != 0)
    {
        //means the vector is non zero
        ent->entity_rotation = (vector2d_angle(ent->velocity) + 180);
//        angle_clamp_radians(&ent->rotation);
    }
}

void gme_entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (!entity_manager.entity_list[i]._entity_inuse)continue;
        gme_entity_update(&entity_manager.entity_list[i]);
    }
}

// void gme_entity_think(Entity *ent)
// {
//     if (!ent)return;
//     if (ent->think)ent->think(ent);
// }

void gme_entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_max;i++)
    {
        if (!entity_manager.entity_list[i]._entity_inuse)continue;
        gme_entity_think(&entity_manager.entity_list[i]);
    }
}

SJson *gme_entity_get_def_by_name(const char *name)
{
//    int i,c;
    if (!name)return NULL;
    
    return NULL;
}
void gme_entity_animate(Entity *ent, float frame_start, float frame_final){
    ent->entity_frame+=0.1;
    if (ent->entity_frame >= frame_final)ent->entity_frame = frame_start;
}

void gme_entity_think(Entity *self)
{
    Vector2D m,dir,camera;
    int mx,my;
    if (!self)return;
    camera = camera_get_position();
    SDL_GetMouseState(&mx,&my);
    m.x = mx;
    m.y = my;
    vector2d_add(m,m,camera);
    vector2d_sub(dir,m,self->position);
    if (vector2d_magnitude_compare(dir,10)>0)
    {
        vector2d_set_magnitude(&dir,self->entity_speed);
        vector2d_copy(self->velocity,dir);
    }
    else
    {
        vector2d_clear(self->velocity);
    }
    camera_center_at(self->position);
}


/*eol@eof*/
