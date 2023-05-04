
#include "sound.h"
#include "globals.h"

extern global_state *g;

// volume is out of 100

/// @brief world = 0 battle=1 hit=2 die=4
/// @param volume
void load_sounds(int volume)
{
    char buf[256];
    for (int i=0; i<NUM_SOUNDS; ++i) {
        snprintf(buf, 256, "./sounds/%d.wav", i);
        if(!(g->sounds[i] = Mix_LoadWAV(buf))) {
            printf("load_sounds: %s\n", Mix_GetError());
            exit(0);
        }
    }
    Mix_AllocateChannels(NUM_CHANNELS);
    Mix_ReserveChannels(1); //TODO for now just 1 for music
    set_volume();
}

void set_volume()
{
    // all channel volume
    Mix_Volume(-1, g->sound_effect_vol/100.0*MIX_MAX_VOLUME);


    // set music volume separately, lower
    Mix_Volume(0, g->music_vol/100.0*MIX_MAX_VOLUME);
}

void free_sounds()
{
    char buf[256];
    for (int i=0; i<NUM_SOUNDS; ++i) {
        Mix_FreeChunk(g->sounds[i]);
    }

    // TODO
    /*
    Mix_AllocateChannels(NUM_CHANNELS);
    Mix_ReserveChannels(1); //TODO for now just 1 for thrusting, not sure about multiplayer
    */
}

void play_music(Mix_Chunk* chunk, Uint32 delay)
{
    // 0 channel is for music, -1 loops "inifinitely"
    int channel = Mix_PlayChannel(0, chunk, -1);
    if (delay) {
        Mix_Pause(channel);
        SDL_Delay(delay);
        Mix_Resume(channel);
    }
}

int play_sound(Mix_Chunk* chunk, Uint32 delay)
{
    int channel = Mix_PlayChannel(-1, chunk, 0);
    if (delay) {
        Mix_Pause(channel);
        SDL_Delay(delay);
        Mix_Resume(channel);
    }
    return channel;
}

