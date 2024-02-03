#include "Rocket.hpp" //implements

#include "Utils.h"

Rocket::Rocket(const float x, const float y,
			   const float fuse)
	: Particle(x, y, fuse,
			   olc::YELLOW)
{
}

std::unique_ptr<Particle> Rocket::CreateNewRocket(const olc::PixelGameEngine& pge)
{
	const float x = random(static_cast<float>(pge.ScreenWidth()));
	const float y = static_cast<float>(pge.ScreenHeight());

	const float strength = random(6500.0f, 11000.0f);
	const float fuse = random(strength * 0.0003f, strength * 0.00055f);

	auto r = std::make_unique<Rocket>(x, y, fuse);
	r->ApplyForce({0.0f, -strength});
	return r;
}

std::vector<std::unique_ptr<Particle>> Rocket::Update(const float fElapsedTime)
{
	Particle::Update(fElapsedTime);
	if (IsFizzledOut())
	{
		return Explode();
	}

	return {};
}

std::vector<std::unique_ptr<Particle>> Rocket::Explode() const
{
	std::vector<std::unique_ptr<Particle>> newSparkles;

	const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
	const olc::Pixel newColour = colours[random(6)];

	for (int i = 0; i < random(10, 20); ++i)
	{
		auto sparkle = CreateNewSparkle(GetPosition(), newColour);
		newSparkles.push_back(std::move(sparkle));
	}

	return newSparkles;
}
