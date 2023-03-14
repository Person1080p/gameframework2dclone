//#include <SDL.h>
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_IMPLEMENTATION
//#define NK_SDL_RENDERER_IMPLEMENTATION
//#include "nuklear.h"
//#include "nuklear_sdl_renderer.h"

#include <SDL_image.h>
#include <stdlib.h>

#include "gf2d_graphics.h"
#include "simple_logger.h"

/*local types*/
typedef struct
{
    SDL_Window   *   main_window;
    SDL_Renderer *   renderer;
    SDL_Texture  *   texture;
    SDL_Surface  *   surface;
    SDL_Surface  *   temp_buffer;

    Uint32 frame_delay;
    Uint32 now;
    Uint32 then;
    Bool print_fps;
    float fps; 

    Uint32 background_color;
    Vector4D background_color_v;

    Sint32 bitdepth;
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    Uint32 renderWidth;
    Uint32 renderHeight;
    
    SDL_BlendMode lightMode;
    SDL_BlendMode subtractMode;
}Graphics;

/*local gobals*/
static Graphics gf2d_graphics;
int    __DebugMode = 0;

/*forward declarations*/
void gf2d_graphics_close();

void gf2d_graphics_initialize(
    char *windowName,
    int viewWidth,
    int viewHeight,
    int renderWidth,
    int renderHeight,
    Vector4D bgcolor,
    Bool fullscreen
)
{
    Uint32 flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        slog("Unable to initilaize SDL system: %s",SDL_GetError());
        return;
    }
    atexit(SDL_Quit);
    if (fullscreen)
    {
        if (renderWidth == 0)
        {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        else
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
    }
    gf2d_graphics.main_window = SDL_CreateWindow(windowName,
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             viewWidth, viewHeight,
                             flags);

    if (!gf2d_graphics.main_window)
    {
        slog("failed to create main window: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    gf2d_graphics.renderer = SDL_CreateRenderer(gf2d_graphics.main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!gf2d_graphics.renderer)
    {
        slog("failed to create renderer: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    SDL_SetRenderDrawColor(gf2d_graphics.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gf2d_graphics.renderer);
    SDL_RenderPresent(gf2d_graphics.renderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(gf2d_graphics.renderer, renderWidth, renderHeight);

    gf2d_graphics.texture = SDL_CreateTexture(
        gf2d_graphics.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        renderWidth, renderHeight);
    if (!gf2d_graphics.texture)
    {
        slog("failed to create screen texture: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888,
                                    &gf2d_graphics.bitdepth,
                                    &gf2d_graphics.rmask,
                                    &gf2d_graphics.gmask,
                                    &gf2d_graphics.bmask,
                                    &gf2d_graphics.amask);
    
    gf2d_graphics.renderWidth = renderWidth;
    gf2d_graphics.renderHeight = renderHeight;
    
    gf2d_graphics.surface = SDL_CreateRGBSurface(0, renderWidth, renderHeight, gf2d_graphics.bitdepth,
                                        gf2d_graphics.rmask,
                                    gf2d_graphics.gmask,
                                    gf2d_graphics.bmask,
                                    gf2d_graphics.amask);
    
    if (!gf2d_graphics.surface)
    {
        slog("failed to create screen surface: %s",SDL_GetError());
        gf2d_graphics_close();
        return;
    }
    
    gf2d_graphics.background_color = SDL_MapRGB(gf2d_graphics.surface->format, bgcolor.x,bgcolor.y,bgcolor.z);
    vector4d_set(gf2d_graphics.background_color_v,bgcolor.x,bgcolor.y,bgcolor.z,bgcolor.w);
    SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),SDL_BLENDMODE_BLEND);

    srand(SDL_GetTicks());
    
    gf2d_graphics.lightMode = SDL_ComposeCustomBlendMode(
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDFACTOR_ZERO,
        SDL_BLENDOPERATION_ADD,
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDOPERATION_MINIMUM);

    gf2d_graphics.subtractMode = SDL_ComposeCustomBlendMode(
        SDL_BLENDFACTOR_SRC_ALPHA,
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDOPERATION_SUBTRACT,
        SDL_BLENDFACTOR_ZERO,
        SDL_BLENDFACTOR_ONE,
        SDL_BLENDOPERATION_SUBTRACT);

    atexit(gf2d_graphics_close);
    slog("graphics initialized");
}

void gf2d_graphics_save_screenshot(const char *filename)
{
    SDL_Surface *surface;
    if (!filename)
    {
        slog("no filename specified for screenshot");
        return;
    }
    surface = gf2d_graphics_get_render();
    if (surface == NULL)
    {
        slog("failed to get surface of the render");
        return;
    }
    if (IMG_SavePNG(surface, filename))
    {
        slog("screenshot failed to save: %s",SDL_GetError());
    }
    SDL_FreeSurface(surface);
}

SDL_Surface *gf2d_graphics_get_render()
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    Uint32 format = 0;
    int w,h;
    texture = SDL_GetRenderTarget(gf2d_graphics_get_renderer());
    if (texture)
    {
        slog("Taking screenshot from a texture target");
        SDL_QueryTexture(texture,
                        &format, NULL,
                        &w, &h);
    }
    else
    {
        w = gf2d_graphics.renderWidth;
        h = gf2d_graphics.renderHeight;
        format = SDL_GetWindowPixelFormat(gf2d_graphics.main_window);
    }
    surface = SDL_CreateRGBSurface(0, w, h, gf2d_graphics.bitdepth,
                                        gf2d_graphics.rmask,
                                    gf2d_graphics.gmask,
                                    gf2d_graphics.bmask,
                                    gf2d_graphics.amask);
    if (!surface)return NULL;
    SDL_LockSurface(surface);
    SDL_RenderReadPixels(gf2d_graphics_get_renderer(),
                             NULL,
                             format,
                             surface->pixels,
                             surface->pitch);
    SDL_UnlockSurface(surface);
    return surface;
}

void gf2d_graphics_set_blend_mode(SDL_BlendMode mode)
{
    SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(),mode);
}

SDL_BlendMode gf2d_graphics_get_light_blend_mode()
{
    return gf2d_graphics.lightMode;
}

SDL_BlendMode gf2d_graphics_get_subtract_blend_mode()
{
//    return SDL_BLENDMODE_BLEND;
    return gf2d_graphics.subtractMode;
}

Uint32 gf2d_graphics_get_image_format()
{
    return gf2d_graphics.surface->format->format;
}

void gf2d_graphics_close()
{
    if (gf2d_graphics.texture)
    {
        SDL_DestroyTexture(gf2d_graphics.texture);
    }
    if (gf2d_graphics.renderer)
    {
        SDL_DestroyRenderer(gf2d_graphics.renderer);
    }
    if (gf2d_graphics.main_window)
    {
        SDL_DestroyWindow(gf2d_graphics.main_window);
    }
    if (gf2d_graphics.surface)
    {
        SDL_FreeSurface(gf2d_graphics.surface);
    }
    if (gf2d_graphics.temp_buffer)
    {
        SDL_FreeSurface(gf2d_graphics.temp_buffer);
    }
    gf2d_graphics.surface = NULL;
    gf2d_graphics.main_window = NULL;
    gf2d_graphics.renderer = NULL;
    gf2d_graphics.texture = NULL;
    gf2d_graphics.temp_buffer = NULL;

    slog("graphics closed");
}

SDL_Renderer *gf2d_graphics_get_renderer()
{
    return gf2d_graphics.renderer;
}

SDL_Texture *gf2d_graphics_get_screen_texture()
{
    return gf2d_graphics.texture;
}

SDL_Surface *gf2d_graphics_get_screen_surface()
{
    return gf2d_graphics.surface;
}

void gf2d_graphics_set_frame_delay(Uint32 frameDelay)
{
    gf2d_graphics.frame_delay = frameDelay;
}

float gf2d_graphics_get_frames_per_second()
{
    return gf2d_graphics.fps;
}

Vector2D gf2d_graphics_get_resolution()
{
    return vector2d((float)gf2d_graphics.renderWidth,(float)gf2d_graphics.renderHeight);
}

void gf2d_graphics_frame_delay()
{
    Uint32 diff;
    gf2d_graphics.then = gf2d_graphics.now;
    slog_sync();// make sure logs get written when we have time to write it
    gf2d_graphics.now = SDL_GetTicks();
    diff = (gf2d_graphics.now - gf2d_graphics.then);
    if (diff < gf2d_graphics.frame_delay)
    {
        SDL_Delay(gf2d_graphics.frame_delay - diff);
    }
    gf2d_graphics.fps = 1000.0/MAX(SDL_GetTicks() - gf2d_graphics.then,0.001);
}

void gf2d_graphics_next_frame()
{
    SDL_RenderPresent(gf2d_graphics.renderer);
    gf2d_graphics_frame_delay();
}

void gf2d_graphics_clear_screen()
{
    if (!gf2d_graphics.surface)
    {
        return;
    }
    SDL_SetRenderDrawColor(
        gf2d_graphics.renderer,
        gf2d_graphics.background_color_v.x,
        gf2d_graphics.background_color_v.y,
        gf2d_graphics.background_color_v.z,
        gf2d_graphics.background_color_v.w);
    SDL_FillRect(gf2d_graphics.surface,NULL,gf2d_graphics.background_color);
    SDL_RenderClear(gf2d_graphics.renderer);
}

SDL_Surface *gf2d_graphics_create_surface(Uint32 w,Uint32 h)
{
    SDL_Surface *surface;
    surface = SDL_CreateRGBSurface(
        0,w, h,
        gf2d_graphics.bitdepth,
        gf2d_graphics.rmask,
        gf2d_graphics.gmask,
        gf2d_graphics.bmask,
        gf2d_graphics.amask);
    return surface;
}

void gf2d_graphics_render_texture_to_screen(SDL_Texture *texture,const SDL_Rect * srcRect,SDL_Rect * dstRect)
{
    if (!texture)return;
    if (!gf2d_graphics.renderer)
    {
        slog("no graphics rendering context");
        return;
    }
    if (SDL_RenderCopy(gf2d_graphics.renderer,
                   texture,
                   srcRect,
                   dstRect))
    {
        slog("failed to render:%s",SDL_GetError());
    }

}

void gf2d_graphics_blit_surface_to_screen(SDL_Surface *surface,const SDL_Rect * srcRect,SDL_Rect * dstRect)
{
    if (!surface)return;
    if (!gf2d_graphics.surface)
    {
        slog("no screen surface loaded");
        return;
    }
    SDL_BlitSurface(surface,
                    srcRect,
                    gf2d_graphics.surface,
                    dstRect);
}

SDL_Surface *gf2d_graphics_screen_convert(SDL_Surface **surface)
{
    SDL_Surface *convert;
    if (!(*surface))
    {
        slog("surface provided was NULL");
        return NULL;
    }
    if (!gf2d_graphics.surface)
    {
        slog("graphics not yet initialized");
        return NULL;
    }
    convert = SDL_ConvertSurface(*surface,
                       gf2d_graphics.surface->format,
                       0);
    if (!convert)
    {
        slog("failed to convert surface: %s",SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(*surface);
    *surface = NULL;
    return convert;
}

struct nk_context* gf2d_nuklear_init()
{
	SDL_Renderer* ren = gf2d_graphics.renderer;
	SDL_Window* win = gf2d_graphics.main_window;
    /* scale the renderer output for High-DPI displays */
    float font_scale;
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(ren, &render_w, &render_h);
        SDL_GetWindowSize(win, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(ren, scale_x, scale_y);
        font_scale = scale_y;
    }

    /* GUI */
    struct nk_context *ctx = nk_sdl_init(win, ren);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        /* set up the font atlas and add desired font; note that font sizes are
         * multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
        /* font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale, &config); */
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
        nk_sdl_font_stash_end();

        /* this hack makes the font appear to be scaled down to the desired
         * size and is only necessary when font_scale > 1 */
        font->handle.height /= font_scale;
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(ctx, &font->handle);
    }

    return ctx;
}

/*eol@eof*/
