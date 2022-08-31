#ifndef FIREWORKSPGE_PARTICLE_H
#define FIREWORKSPGE_PARTICLE_H

#include "olcPixelGameEngine.h"


class Particle
{
public:
	bool exploded;
private:
	olc::PixelGameEngine* pge;
	bool rocket;
	olc::vf2d position;
	olc::vf2d velocity;
	olc::vf2d acceleration;
	float mass;
	float initialFuse;
	float fuse;
	olc::Pixel colour;

public:
	Particle(olc::PixelGameEngine* pge, bool rocket, float x, float y, float fuse, olc::Pixel colour);

	void Update(float fElapsedTime, std::vector<Particle>* sparkles);

	void Render();

	void ApplyForce(const olc::vf2d &force);

	void Explode(std::vector<Particle>* sparkles);
};


#endif //FIREWORKSPGE_PARTICLE_H