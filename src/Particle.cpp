#include "Particle.h"
#include "Utils.h"

Particle Particle::CreateRocket(const olc::PixelGameEngine& pge)
{
	const float x = random(static_cast<float>(pge.ScreenWidth()));
	const float y = static_cast<float>(pge.ScreenHeight());

	const float strength = random(6500.0f, 11000.0f);
	const float fuse = random(strength * 0.0003f, strength * 0.00055f);

	Particle p(true, x, y, fuse, olc::YELLOW);
	p.ApplyForce({0.0f, -strength});
	return p;
}

Particle::Particle(const bool isRocket,
				   const float x, const float y,
				   const float fuse,
				   const olc::Pixel colour)
	: isRocket{isRocket},
	  position{x, y}, velocity{0.0f, 0.0f}, acceleration{0.0f, 0.0f},
	  initialFuse{fuse}, fuse{fuse},
	  colour{colour}
{
}

void Particle::ApplyForce(const olc::vf2d& force)
{
	acceleration += 1.0f / mass * force;
}

void Particle::Update(const float fElapsedTime, std::vector<Particle>& sparkles)
{
	//it's here twice: https://web.archive.org/web/20160307185547/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
	velocity += fElapsedTime / 2 * acceleration;
	position += velocity * fElapsedTime;
	velocity += fElapsedTime / 2 * acceleration;
	acceleration = {0.0f, 0.0f};

	fuse -= fElapsedTime;
	if (fuse < 0.0f)
	{
		isExploded = true;
		if (isRocket)
			Explode(sparkles);
	}
}

void Particle::Render(olc::PixelGameEngine& pge)
{
	colour.a = static_cast<uint8_t>(255.0f * (fuse / initialFuse));
	pge.Draw(position, colour);
}

bool Particle::IsExploded() const
{
	return isExploded;
}

void Particle::Explode(std::vector<Particle>& sparkles) const
{
	const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
	const olc::Pixel newColour = colours[random(6)];

	for (int i = 0; i < random(10, 20); ++i)
	{
		Particle sparkle(false, position.x, position.y, random(0.8f, 1.2f), newColour);
		olc::vf2d direction = {random(-1.0f, 1.0f), random(-1.0f, 1.0f)};
		direction = direction.norm();
		sparkle.ApplyForce(direction * random(5000.0f, 8000.0f));

		sparkles.push_back(sparkle);
	}
}
