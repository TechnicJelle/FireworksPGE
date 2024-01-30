#ifndef FIREWORKSPGE_UTILS_H
#define FIREWORKSPGE_UTILS_H

#include "olcPixelGameEngine.h"

//TODO: Not inline these
//Call this once, before ever calling one of these `random()` functions
inline void randomInit() {
	srandom(time(nullptr));
}

inline float random(const float max)
{
	return static_cast<float>(random()) / static_cast<float>(RAND_MAX) * max;
}

inline float random(const float min, const float max)
{
	return static_cast<float>(random()) / static_cast<float>(RAND_MAX) * (max - min) + min;
}

inline long random(const int max)
{
	return random() % max;
}

inline long random(const int min, const int max)
{
	return random() % (max - min) + min;
}

#endif //FIREWORKSPGE_UTILS_H
