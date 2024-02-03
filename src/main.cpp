#include "olcPixelGameEngine.h"
#include <entt/entt.hpp>

#include "Utils.h"

constexpr bool vsync = false;
constexpr float physicsFPS = 200.0f;
constexpr float dt = 1.0f / physicsFPS;

class Physics
{
	olc::vf2d position = {0.0f, 0.0f};
	olc::vf2d velocity = {0.0f, 0.0f};
	olc::vf2d acceleration = {0.0f, 0.0f};
	float mass = 1.0f;

public:
	explicit Physics(const olc::vf2d& position)
		: position(position)
	{
	}

	void Update(const float fElapsedTime)
	{
		//it's here twice, because: https://web.archive.org/web/20160307185547/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html
		const float halfTime = fElapsedTime / 2.0f;
		velocity += halfTime * acceleration;
		position += velocity * fElapsedTime;
		velocity += halfTime * acceleration;
		acceleration = {0.0f, 0.0f};
	}

	void ApplyForce(const olc::vf2d& force)
	{
		acceleration += 1.0f / mass * force;
	}

	[[nodiscard]] olc::vf2d GetPosition() const
	{
		return position;
	}
};

class Renderer
{
	olc::Pixel colour = olc::WHITE;
	float initialFuseTime;
	float fuseTime;

public:
	explicit Renderer(const olc::Pixel colour, const float fuseTime)
		: colour(colour), initialFuseTime(fuseTime), fuseTime(fuseTime)
	{
	}

	[[nodiscard]] olc::Pixel GetColour() const
	{
		return colour;
	}

	void DecrementFuseTime(const float fElapsedTime)
	{
		fuseTime -= fElapsedTime;
		colour.a = static_cast<uint8_t>(fuseTime / initialFuseTime * 255); // fade out
	}

	[[nodiscard]] bool IsFuseExpired() const
	{
		return fuseTime <= 0.0f;
	}
};

struct Expired
{
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

	void PhysicsAndFuseSystem()
	{
		const auto physicsView = registry.view<Physics, Renderer>();
		physicsView.each([this](const entt::entity entity, Physics& phys, Renderer& rend)
		{
			phys.ApplyForce(gravityForce);
			phys.Update(dt);

			rend.DecrementFuseTime(dt);
			if (rend.IsFuseExpired())
			{
				registry.emplace<Expired>(entity);
			}
		});
	}

	void ExplodeSystem()
	{
		const auto explodeView = registry.view<const Expired, const Explode, const Physics>();
		explodeView.each([this](const Explode& explode, const Physics& phys)
		{
			for (uint8_t i = 0; i < explode.numSparkles; i++)
			{
				CreateNewSparkle(registry, phys.GetPosition(), explode.colour);
			}

			CreateNewRocket(registry, *this);
		});
	}

	void CleanupSystem()
	{
		const auto expiredView = registry.view<Expired>();
		registry.destroy(std::begin(expiredView), std::end(expiredView));
	}

	void RenderingSystem()
	{
		const auto renderView = registry.view<const Physics, const Renderer>();
		renderView.each([this](const Physics& phys, const Renderer& rend)
		{
			Draw(phys.GetPosition(), rend.GetColour());
		});
	}

	bool OnUserUpdate(const float fElapsedTime) override
	{
		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			return false; // quit
		}

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

			PhysicsAndFuseSystem();

			ExplodeSystem();

			CleanupSystem();
		}

		Clear(olc::BLACK);
		RenderingSystem();

		return true;
	}

	static void CreateNewRocket(entt::registry& registry, const PixelGameEngine& pge)
	{
		const entt::entity rocket = registry.create();

		const float x = random(static_cast<float>(pge.ScreenWidth()));
		const float y = static_cast<float>(pge.ScreenHeight());

		const float launchStrength = random(6500.0f, 11000.0f);
		Physics& phys = registry.emplace<Physics>(rocket, olc::vf2d(x, y));
		phys.ApplyForce({0.0f, -launchStrength});

		const float fuseTime = random(launchStrength * 0.0003f, launchStrength * 0.00055f);
		registry.emplace<Renderer>(rocket, olc::YELLOW, fuseTime);

		const uint8_t numSparkles = static_cast<uint8_t>(random(10, 20));
		const olc::Pixel colours[] = {olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN};
		const olc::Pixel sparklesColour = colours[random(6)];
		registry.emplace<Explode>(rocket, numSparkles, sparklesColour);
	}

	static void CreateNewSparkle(entt::registry& registry, const olc::vf2d& position, olc::Pixel colour)
	{
		const entt::entity sparkle = registry.create();

		Physics& phys = registry.emplace<Physics>(sparkle, position);
		olc::vf2d direction = {random(-1.0f, 1.0f), random(-1.0f, 1.0f)};
		direction = direction.norm();
		phys.ApplyForce(direction * random(5000.0f, 8000.0f));

		const float fuseTime = random(0.8f, 1.2f);
		registry.emplace<Renderer>(sparkle, colour, fuseTime);
	}
};


int main()
{
	if (FireworksPGE game; game.Construct(320, 180, 4, 4, false, vsync))
		game.Start();

	return 0;
}
