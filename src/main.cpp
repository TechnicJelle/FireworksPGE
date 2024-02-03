#include "olcPixelGameEngine.h"
#include "Particle.h"
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

	std::array<Rocket, 50> rockets;
	std::vector<Particle> sparkles;

	float accumulator = 0.0f;

public:
	bool OnUserCreate() override
	{
		randomInit();

		for (Rocket& r : rockets)
		{
			r = Rocket::CreateNewRocket(*this);
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
			for (Rocket& r : rockets)
			{
				r.ApplyForce(gravity);
				r.Update(dt, sparkles);

				if (r.IsExploded())
				{
					r = Rocket::CreateNewRocket(*this);
				}
			}
			for (Particle& s : sparkles)
			{
				s.ApplyForce(gravity);
				s.Update(dt);
			}

			//remove all exploded particles from the sparkles vector
			sparkles.erase(
				std::remove_if(
					sparkles.begin(),
					sparkles.end(),
					[](const Particle& i) { return i.IsExploded(); }
				),
				sparkles.end()
			);
		}

#pragma endregion // Physics


#pragma region Rendering

		Clear(olc::BLACK);
		for (Rocket& r : rockets)
		{
			r.Render(*this);
		}
		for (Particle& s : sparkles)
		{
			s.Render(*this);
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
