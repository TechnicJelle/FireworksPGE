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

class Fuse
{
	float initialTime;
	float time;

public:
	explicit Fuse(const float time) : initialTime(time), time(time)
	{
	}

	void DecrementTime(const float dt)
	{
		time -= dt;
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

struct Renderer
{
	olc::Pixel colour = olc::WHITE;
	int32_t radius = 1;
};

class FireworksPGE final : public olc::PixelGameEngine
{
public:
	FireworksPGE()
	{
		sAppName = "Fireworks";
	}

private:
	const olc::vf2d gravity = {0.0f, 10.0f};

	entt::registry registry;
	float accumulator = 0.0f;

	bool OnUserCreate() override
	{
		randomInit();

		for (int i = 0; i < 50; i++)
		{
			CreateRocket(registry);
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
					mov.ApplyForce(gravity);
					mov.Update(pos, dt);
				}

				const auto fuseView = registry.view<Fuse>();
				for (auto [entity, fuse] : fuseView.each())
				{
					fuse.DecrementTime(dt);
					if (fuse.IsExpired())
					{
						registry.destroy(entity);
						CreateRocket(registry);
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
				FillCircle(pos.position, rend.radius, colour);
			}
		}
#pragma endregion // Rendering

		return true;
	}

	void CreateRocket(entt::registry& registry) const
	{
		const entt::entity entity = registry.create();

		const float x = random(static_cast<float>(ScreenWidth()));
		const float y = static_cast<float>(ScreenHeight());
		registry.emplace<Position>(entity, olc::vf2d(x, y));

		registry.emplace<Renderer>(entity, olc::YELLOW, 0);

		Movement& mov = registry.emplace<Movement>(entity);
		const float strength = random(6500.0f, 11000.0f);
		mov.ApplyForce({0.0f, -strength});

		const float fuse = random(strength * 0.0003f, strength * 0.00055f);
		registry.emplace<Fuse>(entity, fuse);
	}
};


int main()
{
	if (FireworksPGE game; game.Construct(320, 180, 4, 4, false, vsync))
		game.Start();

	return 0;
}
