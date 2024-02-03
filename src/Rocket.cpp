#include "Rocket.hpp"
#include "Utils.h"

Rocket::Rocket(const float x, const float y,
			   const float fuse)
	: Particle(x, y, fuse,
			   olc::YELLOW)
{
}

Rocket Rocket::CreateNewRocket(const olc::PixelGameEngine& pge)
{
	const float x = random(static_cast<float>(pge.ScreenWidth()));
	const float y = static_cast<float>(pge.ScreenHeight());

	const float strength = random(6500.0f, 11000.0f);
	const float fuse = random(strength * 0.0003f, strength * 0.00055f);

	Rocket r(x, y, fuse);
	r.ApplyForce({0.0f, -strength});
	return r;
}

void Rocket::Update(const float fElapsedTime, std::vector<Particle>& sparkles)
{
	Particle::Update(fElapsedTime);
	if (IsExploded())
	{
		Explode(sparkles);
	}
}

void Rocket::Explode(std::vector<Particle>& sparkles) const
{
	const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
	const olc::Pixel newColour = colours[random(6)];

	for (int i = 0; i < random(10, 20); ++i)
	{
		Particle sparkle = CreateNewSparkle(GetPosition(), newColour);
		sparkles.push_back(sparkle);
	}
}
