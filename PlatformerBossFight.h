#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

enum class PlatformerBossResult
{
	Running,
	PlayerWon,
	PlayerLost,
	Cancelled
};

class PlatformerBossFight
{
public:
	void start(const sf::Vector2u& windowSize);
	void handleEvent(const sf::Event& event);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window);

	bool isActive() const;
	PlatformerBossResult getResult() const;
	void stop();

private:
	enum class ProjectileType
	{
		SineWave,
		Bouncing
	};

	struct BossProjectile
	{
		sf::CircleShape shape;
		sf::Vector2f velocity{ 0.0f, 0.0f };

		ProjectileType type = ProjectileType::SineWave;

		float age = 0.0f;
		float centerY = 0.0f;
		float waveAmplitude = 45.0f;
		float waveFrequency = 6.0f;
		int remainingBounces = 3;
	};

	void updatePlayer(float deltaTime);
	void updateBoss(float deltaTime);
	void updateBossProjectiles(float deltaTime);
	void updatePlayerAttack(float deltaTime);
	void drawPlayerAttack(sf::RenderWindow& window) const;
	bool jumpKeyWasPressed = false;
	bool attackKeyWasPressed = false;
	sf::RectangleShape playerAttackShape;

	bool playerAttackVisible = false;
	float playerAttackTimer = 0.0f;
	void spawnSineProjectile();
	void spawnBouncingProjectile();

	// Trajectory warning
	void drawAttackTrajectory(sf::RenderWindow& window) const;

	void handleCollisions();
	void damagePlayer(int damage);
	void damageBoss(int damage);

	sf::RectangleShape player;
	sf::RectangleShape boss;
	sf::RectangleShape ground;
	sf::RectangleShape platform;

	std::vector<BossProjectile> bossProjectiles;

	sf::Vector2f playerVelocity{ 0.0f, 0.0f };
	sf::Vector2f bossVelocity{ 100.0f, 0.0f };

	bool active = false;
	bool playerOnGround = false;
	bool attackPressed = false;
	bool nextAttackIsSine = true;

	// These belong here
	bool showingTrajectory = false;
	float trajectoryTimer = 0.0f;

	int playerHealth = 5;
	int bossHealth = 10;

	float projectileTimer = 0.0f;
	float playerAttackCooldown = 0.0f;
	float playerDamageCooldown = 0.0f;

	PlatformerBossResult result =
		PlatformerBossResult::Cancelled;

	sf::Vector2u arenaSize{ 800, 600 };
};