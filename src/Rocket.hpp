#ifndef ROCKET_HPP
#define ROCKET_HPP

#include "Particle.h"


class Rocket : public Particle {
	Rocket(float x, float y, float fuse);

public:
	Rocket() = default;

	static Rocket CreateNewRocket(const olc::PixelGameEngine& pge);

	void Update(float fElapsedTime, std::vector<Particle>& sparkles);

private:
	void Explode(std::vector<Particle>& sparkles) const;
};


#endif //ROCKET_HPP
