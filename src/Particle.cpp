#include "Particle.h"
#include "Utils.h"

Particle::Particle(const float x, const float y,
				   const float fuse,
				   const olc::Pixel colour)
	: position{x, y},
	  initialFuse{fuse}, fuse{fuse},
	  colour{colour}
{
}

Particle Particle::CreateNewSparkle(const olc::vf2d& position, const olc::Pixel colour)
{
	Particle sparkle(position.x, position.y, random(0.8f, 1.2f), colour);

	olc::vf2d direction = {random(-1.0f, 1.0f), random(-1.0f, 1.0f)};
	direction = direction.norm();
	sparkle.ApplyForce(direction * random(5000.0f, 8000.0f));

	return sparkle;
}

void Particle::ApplyForce(const olc::vf2d& force)
{
	acceleration += 1.0f / mass * force;
}

void Particle::Update(const float fElapsedTime)
{
	//it's here twice: https://web.archive.org/web/20160307185547/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
	velocity += fElapsedTime / 2 * acceleration;
	position += velocity * fElapsedTime;
	velocity += fElapsedTime / 2 * acceleration;
	acceleration = {0.0f, 0.0f};

	fuse -= fElapsedTime;
}

void Particle::Render(olc::PixelGameEngine& pge)
{
	colour.a = static_cast<uint8_t>(255.0f * (fuse / initialFuse));
	pge.Draw(position, colour);
}

bool Particle::IsExploded() const
{
	return fuse <= 0.0f;
}

olc::vf2d Particle::GetPosition() const
{
	return position;
}
