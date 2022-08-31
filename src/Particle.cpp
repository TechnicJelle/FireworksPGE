#include "Particle.h"
#include "Utils.h"

Particle::Particle(olc::PixelGameEngine* pge, bool rocket, float x, float y, float fuse)
{
	this->pge = pge;
	this->rocket = rocket;
	position = {x, y};
	velocity = {0.0f, 0.0f};
	acceleration = {0.0f, 0.0f};
	mass = 1.0f;
	this->fuse = fuse;
	exploded = false;
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
	if (rocket && fuse < 0.0f)
	{
		Explode(sparkles);
	}
}

void Particle::Render()
{
	pge->Draw(position, olc::RED);
}

void Particle::Explode(std::vector<Particle>* sparkles)
{
	std::cout << "Explode" << std::endl;
	for (int i = 0; i < 1; ++i)
	{
		Particle sparkle = Particle(pge, false, position.x, position.y, 1.0f);
		sparkle.ApplyForce({0.0f, -random(1000, 2000)});

		sparkles->push_back(sparkle);
	}
	exploded = true;
}