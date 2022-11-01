#ifndef FIREWORKSPGE_UTILS_H
#define FIREWORKSPGE_UTILS_H

#include "olcPixelGameEngine.h"

//TODO: Not inline these
//Call this once, before ever calling one of these `random()` functions
inline void randomInit() {
	srandom(time(nullptr));
}

inline float random(float Max)
{
	return (float(random()) / float(RAND_MAX)) * Max;
}

inline float random(float Min, float Max)
{
	return ((float(random()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

#endif //FIREWORKSPGE_UTILS_H
