#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

#include "olcPixelGameEngine.h"
#include "Particle.h"
#include "Utils.h"

constexpr bool vsync = false;
constexpr float physicsFPS = 200.0f;
constexpr float dt = 1.0f / physicsFPS;

class FireworksPGE final : public olc::PixelGameEngine
{
public:
	FireworksPGE()
		: rockets{}
	{
		sAppName = "Fireworks";
	}

	std::vector<Particle> sparkles;

private:
	Particle* rockets[50];
	const olc::vf2d gravity = {0.0f, 10.0f};

	Particle* CreateRocket()
	{
		const float x = random(static_cast<float>(ScreenWidth()));
		const float strength = random(6500.0f, 11000.0f);
		const float fuse = random(strength * 0.0003f, strength * 0.00055f);

		Particle* p = new Particle(this, true, x, static_cast<float>(ScreenHeight()), fuse, olc::YELLOW);
		p->ApplyForce({0.0f, -strength});
		return p;
	}

public:
	bool OnUserCreate() override
	{
		for (Particle*& i : rockets)
		{
			i = CreateRocket();
		}

		sparkles = std::vector<Particle>();

		return true;
	}

	float accumulator = 0.0f;

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
			for (Particle*& r : rockets)
			{
				r->ApplyForce(gravity);
				r->Update(dt, &sparkles);

				if (r->exploded)
				{
					r = CreateRocket();
				}
			}
			for (Particle& s : sparkles)
			{
				s.ApplyForce(gravity);
				s.Update(dt, &sparkles);
			}

			//remove all exploded particles from the sparkles vector
			sparkles.erase(std::remove_if(sparkles.begin(), sparkles.end(),
										  [](const Particle& i) { return i.exploded; }), sparkles.end());
		}

#pragma endregion // Physics


#pragma region Rendering

		Clear(olc::BLACK);
		for (Particle*& r : rockets)
		{
			r->Render();
		}
		for (Particle s : sparkles)
		{
			s.Render();
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


#pragma clang diagnostic pop
