#ifndef FIREWORKSPGE_ROCKET_HPP
#define FIREWORKSPGE_ROCKET_HPP


#include "Particle.hpp" //extends


class Rocket final : public Particle
{
public:
	Rocket() = default;

	Rocket(float x, float y, float fuse);

	static std::unique_ptr<Particle> CreateNewRocket(const olc::PixelGameEngine& pge);

	std::vector<std::unique_ptr<Particle>> Update(float fElapsedTime) override;

private:
	[[nodiscard]] std::vector<std::unique_ptr<Particle>> Explode() const;
};


#endif //FIREWORKSPGE_ROCKET_HPP
