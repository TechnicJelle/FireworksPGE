#include "Particle.h"
#include "Utils.h"

Particle::Particle(olc::PixelGameEngine* pge, bool rocket, float x, float y, float fuse, olc::Pixel colour)
{
	this->pge = pge;
	this->rocket = rocket;
	position = {x, y};
	velocity = {0.0f, 0.0f};
	acceleration = {0.0f, 0.0f};
	mass = 1.0f;
	initialFuse = fuse;
	this->fuse = initialFuse;
	exploded = false;
	this->colour = colour;
}

void Particle::ApplyForce(const olc::vf2d &force)
{
	acceleration += 1.0f / mass * force;
}

void Particle::Update(float fElapsedTime, std::vector<Particle>* sparkles)
{
	//it's here twice: https://web.archive.org/web/20160307185547/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
	velocity += fElapsedTime / 2 * acceleration;
	position += velocity * fElapsedTime;
	velocity += fElapsedTime / 2 * acceleration;
	acceleration = {0.0f, 0.0f};

	fuse -= fElapsedTime;
	if (fuse < 0.0f)
	{
		exploded = true;
		if (rocket)
			Explode(sparkles);
	}
}

void Particle::Render()
{
	colour.a = (unsigned char)(255.0f * (fuse / initialFuse));
	pge->Draw(position, colour);
}

void Particle::Explode(std::vector<Particle>* sparkles)
{
	const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
	olc::Pixel newColour = colours[rand() % 6];

	for (int i = 0; i < (int)random(8, 13); ++i)
	{
		Particle sparkle = Particle(pge, false, position.x, position.y, random(0.8f, 1.2f), newColour);
		sparkle.ApplyForce({random(-1000, 1000), random(-1000, 1000)});

		sparkles->push_back(sparkle);
	}
}