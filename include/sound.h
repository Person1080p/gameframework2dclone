#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>

#define NUM_CHANNELS    16

// No idea why
#define DSP_FREQUENCY   11025

enum {
    WORLD_MUS,
    BATTLE_MUS,
    ATTACK,
    DEATH,
    LOSE,
    NUM_SOUNDS
};

int play_sound(Mix_Chunk* chunk, Uint32 delay);
void play_music(Mix_Chunk* chunk, Uint32 delay);
void set_volume();
void load_sounds(int volume);
void free_sounds();



#endif
