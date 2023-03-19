#include "event.h"

char* event_rand_level(int min, int max)
{
    char* ret = (char*)malloc(1024 * sizeof(char));
    int num = (rand() %(max - min + 1)) + min;
    snprintf(ret, 1024, "config/levels/level%d.json", num);
    slog(ret);
    return ret;
}
char* event_rand_chest(int min, int max)
{
    char* ret = (char*)malloc(1024 * sizeof(char));
    int num = (rand() %(max - min + 1)) + min;
    snprintf(ret, 1024, "config/chests/chest%d.json", num);
    slog(ret);
    return ret;
}
