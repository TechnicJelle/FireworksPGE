#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

#include "olcPixelGameEngine.h"
#include "Particle.h"
#include "Utils.h"

const bool vsync = false;
const float physicsFPS = 200;
const float dt = 1 / physicsFPS;

class FireworksPGE : public olc::PixelGameEngine
{
public:
	FireworksPGE()
	{
		sAppName = "Fireworks";
	}

	std::vector<Particle> sparkles;
private:
	Particle* rockets[100];
	const olc::vf2d gravity = {0.0f, 10.0f};

	Particle* CreateRocket()
	{
		float x = random((float) ScreenWidth());
		float strength = random(6500, 11000);
		float fuse = random(strength * 0.0003f, strength * 0.00055f);

		Particle* p = new Particle(this, true, x, (float) ScreenHeight(), fuse);
		p->ApplyForce({0.0f, -strength});
		return p;
	}

public:
	bool OnUserCreate() override
	{
		for (Particle*&i : rockets)
		{
			i = CreateRocket();
		}

		sparkles = std::vector<Particle>();

		return true;
	}

	float accumulator = 0.0f;

	bool OnUserUpdate(float fElapsedTime) override
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
			for (Particle*&i : rockets)
			{
				i->ApplyForce(gravity);
				i->Update(dt, &sparkles);

				if (i->exploded)
				{
					i = CreateRocket();
				}
			}
			for (Particle &i : sparkles)
			{
				i.ApplyForce(gravity);
				i.Update(dt, &sparkles);
			}
		}

#pragma endregion // Physics


#pragma region Rendering

		Clear(olc::BLACK);
		for (Particle*&i : rockets)
		{
			i->Render();
		}
		for (Particle i : sparkles)
		{
			i.Render();
		}

#pragma endregion // Rendering

		return true;
	}
};


int main()
{
	FireworksPGE game;
	if (game.Construct(320, 180, 4, 4, false, vsync))
		game.Start();

	return 0;
}


#pragma clang diagnostic pop