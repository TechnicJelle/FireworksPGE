#include "olcPixelGameEngine.h"
#include <entt/entt.hpp>

#include "Utils.h"

constexpr bool vsync = false;
constexpr float physicsFPS = 200.0f;
constexpr float dt = 1.0f / physicsFPS;

struct Position
{
	olc::vf2d position = {0.0f, 0.0f};
};

class Movement
{
	olc::vf2d velocity = {0.0f, 0.0f};
	olc::vf2d acceleration = {0.0f, 0.0f};
	float mass = 1.0f;

public:
	void Update(Position& pos, const float fElapsedTime)
	{
		//it's here twice, because: https://web.archive.org/web/20160307185547/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
		const float halfTime = fElapsedTime / 2.0f;
		velocity += halfTime * acceleration;
		pos.position += velocity * fElapsedTime;
		velocity += halfTime * acceleration;
		acceleration = {0.0f, 0.0f};
	}

	void ApplyForce(const olc::vf2d& force)
	{
		acceleration += 1.0f / mass * force;
	}
};

struct Renderer
{
	olc::Pixel colour = olc::WHITE;
};

class Fuse
{
	float initialTime;
	float time;

public:
	explicit Fuse(const float time) : initialTime(time), time(time)
	{
	}

	void DecrementTime(const float fElapsedTime)
	{
		time -= fElapsedTime;
	}

	[[nodiscard]] bool IsExpired() const
	{
		return time <= 0.0f;
	}

	[[nodiscard]] float GetFraction() const
	{
		return time / initialTime;
	}
};

struct Explode
{
	uint8_t numSparkles = 1;
	olc::Pixel colour = olc::WHITE;
};

class FireworksPGE final : public olc::PixelGameEngine
{
public:
	FireworksPGE()
	{
		sAppName = "Fireworks";
	}

private:
	const olc::vf2d gravityForce = {0.0f, 10.0f};

	entt::registry registry;
	float accumulator = 0.0f;

	bool OnUserCreate() override
	{
		randomInit();

		for (uint32_t i = 0; i < 50; i++)
		{
			CreateNewRocket(registry, *this);
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
		{
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
				const auto movementView = registry.view<Movement, Position>();
				for (auto [entity, mov, pos] : movementView.each())
				{
					mov.ApplyForce(gravityForce);
					mov.Update(pos, dt);
				}

				const auto fuseView = registry.view<Fuse>();
				for (auto [entity, fuse] : fuseView.each())
				{
					fuse.DecrementTime(dt);
				}

				const auto explodeView = registry.view<const Fuse, Explode, const Position>();
				for (auto [entity, fuse, explode, pos] : explodeView.each())
				{
					if (fuse.IsExpired())
					{
						for (uint8_t i = 0; i < explode.numSparkles; i++)
						{
							CreateNewSparkle(registry, pos.position, explode.colour);
						}

						CreateNewRocket(registry, *this);
					}
				}

				//Clean up expired entities
				for (auto [entity, fuse] : fuseView.each())
				{
					if (fuse.IsExpired())
					{
						registry.destroy(entity);
					}
				}
			}
		}
#pragma endregion // Physics

#pragma region Rendering
		{
			Clear(olc::BLACK);

			const auto view = registry.view<const Position, const Renderer, const Fuse>();
			for (auto [entity, pos, rend, fuse] : view.each())
			{
				olc::Pixel colour = rend.colour; // copy
				colour.a = static_cast<uint8_t>(fuse.GetFraction() * 255); // fade out
				Draw(pos.position, colour);
			}
		}
#pragma endregion // Rendering

		return true;
	}

	static void CreateNewRocket(entt::registry& registry, const PixelGameEngine& pge)
	{
		const entt::entity rocket = registry.create();

		const float x = random(static_cast<float>(pge.ScreenWidth()));
		const float y = static_cast<float>(pge.ScreenHeight());
		registry.emplace<Position>(rocket, olc::vf2d(x, y));

		registry.emplace<Renderer>(rocket, olc::YELLOW);

		Movement& mov = registry.emplace<Movement>(rocket);
		const float launchStrength = random(6500.0f, 11000.0f);
		mov.ApplyForce({0.0f, -launchStrength});

		const float fuse = random(launchStrength * 0.0003f, launchStrength * 0.00055f);
		registry.emplace<Fuse>(rocket, fuse);

		const uint8_t numSparkles = static_cast<uint8_t>(random(10, 20));
		const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
		const olc::Pixel sparklesColour = colours[random(6)];
		registry.emplace<Explode>(rocket, numSparkles, sparklesColour);
	}

	static void CreateNewSparkle(entt::registry& registry, const olc::vf2d& position, olc::Pixel colour)
	{
		const entt::entity sparkle = registry.create();

		registry.emplace<Position>(sparkle, position);

		registry.emplace<Renderer>(sparkle, colour);

		Movement& mov = registry.emplace<Movement>(sparkle);
		olc::vf2d direction = {random(-1.0f, 1.0f), random(-1.0f, 1.0f)};
		direction = direction.norm();
		mov.ApplyForce(direction * random(5000.0f, 8000.0f));


		const float fuseTime = random(0.8f, 1.2f);
		registry.emplace<Fuse>(sparkle, fuseTime);
	}
};


int main()
{
	if (FireworksPGE game; game.Construct(320, 180, 4, 4, false, vsync))
		game.Start();

	return 0;
}
