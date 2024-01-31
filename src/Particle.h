#ifndef FIREWORKSPGE_PARTICLE_H
#define FIREWORKSPGE_PARTICLE_H

#include "olcPixelGameEngine.h"


class Particle
{
	bool isExploded = false;
	bool isRocket = true;
	olc::vf2d position;
	olc::vf2d velocity;
	olc::vf2d acceleration;
	float mass = 1.0f;
	float initialFuse{};
	float fuse{};
	olc::Pixel colour;

public:
	Particle() = default;

	static Particle CreateRocket(const olc::PixelGameEngine& pge);

	Particle(bool isRocket, float x, float y, float fuse, olc::Pixel colour);

	void Update(float fElapsedTime, std::vector<Particle>& sparkles);

	void Render(olc::PixelGameEngine& pge);

	void ApplyForce(const olc::vf2d& force);

	[[nodiscard]] bool IsExploded() const;

private:
	void Explode(std::vector<Particle>& sparkles) const;
};


#endif //FIREWORKSPGE_PARTICLE_H
