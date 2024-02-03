#include <olcPixelGameEngine.h>

#include "Particle.hpp"
#include "Rocket.hpp"
#include "Utils.h"

constexpr bool vsync = false;
constexpr float physicsFPS = 200.0f;
constexpr float dt = 1.0f / physicsFPS;

class FireworksPGE final : public olc::PixelGameEngine
{
public:
	FireworksPGE()
	{
		sAppName = "Fireworks";
	}

private:
	const olc::vf2d gravity = {0.0f, 10.0f};

	std::vector<std::unique_ptr<Particle>> particles;

	float accumulator = 0.0f;

public:
	bool OnUserCreate() override
	{
		randomInit();

		constexpr int numRockets = 50;
		particles.reserve(numRockets);
		for (int i = 0; i < numRockets; ++i)
		{
			particles.push_back(Rocket::CreateNewRocket(*this));
		}

		return true;
	}

	bool OnUserUpdate(const float fElapsedTime) override
	{
		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			return false; // quit
		}

#pragma region Physics

		//https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-core-engine--gamedev-7493#timestepping
		// Store the time elapsed since the last frame began
		accumulator += fElapsedTime;

		// Avoid spiral of death and clamp dt, thus clamping
		// how many times the UpdatePhysics can be called in
		// a single game loop.
		if (accumulator > 0.2f)
			accumulator = 0.2f;

		while (accumulator >= dt)
		{
			accumulator -= dt;

			//not using an iterator (for-each), because we add stuff to the vector, which resizes it, invalidating the iterator
			for (int i = 0; i < particles.size(); i++)
			{
				const auto& p = particles[i];

				p->ApplyForce(gravity);
				auto newParticles = p->Update(dt);

				if (!newParticles.empty()) //a rocket has exploded
				{
					//so we add a new rocket to replace the exploded one
					newParticles.push_back(Rocket::CreateNewRocket(*this));
				}

				particles.insert(
					particles.end(),
					std::make_move_iterator(newParticles.begin()),
					std::make_move_iterator(newParticles.end())
				);
			}

			//remove all exploded particles from the sparkles vector
			//TODO: Replace with C++ 20 erase_if
			particles.erase(
				std::remove_if(
					particles.begin(),
					particles.end(),
					[](const std::unique_ptr<Particle>& p) { return p->IsFizzledOut(); }
				),
				particles.end()
			);
		}

#pragma endregion // Physics


#pragma region Rendering

		Clear(olc::BLACK);
		for (const auto& p : particles)
		{
			p->Render(*this);
		}

#pragma endregion // Rendering

		return true;
	}
};


int main()
{
	if (FireworksPGE game; game.Construct(320, 180, 4, 4, false, vsync))
		game.Start();

	return 0;
}
