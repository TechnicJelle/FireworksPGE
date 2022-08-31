#ifndef FIREWORKSPGE_UTILS_H
#define FIREWORKSPGE_UTILS_H
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc50-cpp"

#include "olcPixelGameEngine.h"

float random(float Max)
{
	return (float(rand()) / float(RAND_MAX)) * Max;
}

float random(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

#pragma clang diagnostic pop

#endif //FIREWORKSPGE_UTILS_H
