#ifndef FIREWORKSPGE_PARTICLE_HPP
#define FIREWORKSPGE_PARTICLE_HPP


#include <olcPixelGameEngine.h> //draws with


class Particle
{
	olc::vf2d position;
	olc::vf2d velocity;
	olc::vf2d acceleration;
	float mass = 1.0f;
	float initialFuse{};
	float fuse{};
	olc::Pixel colour;

protected:
	Particle(float x, float y, float fuse, olc::Pixel colour);

public:
	Particle();

	static std::unique_ptr<Particle> CreateNewSparkle(const olc::vf2d& position, olc::Pixel colour);

	virtual std::vector<std::unique_ptr<Particle>> Update(float fElapsedTime);

	void Render(olc::PixelGameEngine& pge);

	void ApplyForce(const olc::vf2d& force);

	[[nodiscard]] bool IsFizzledOut() const;

protected:
	[[nodiscard]] olc::vf2d GetPosition() const;
};


#endif //FIREWORKSPGE_PARTICLE_HPP
