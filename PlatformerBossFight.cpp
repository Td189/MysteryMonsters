#include "PlatformerBossFight.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>

namespace
{
	constexpr float PlayerSpeed = 250.0f;
	constexpr float JumpSpeed = 520.0f;
	constexpr float Gravity = 1200.0f;

	constexpr float BossSpeed = 110.0f;

	constexpr float ProjectileInterval = 1.25f;
	constexpr float TrajectoryWarningTime = 1.50f;

	constexpr float SineProjectileSpeed = 310.0f;

	constexpr float BounceProjectileSpeed = 250.0f;
	constexpr float BounceProjectileJumpSpeed = 470.0f;
	constexpr float ProjectileGravity = 1050.0f;

	constexpr float AttackCooldown = 0.40f;
	constexpr float AttackVisibleTime = 0.15f;
	constexpr float AttackWidth = 65.0f;
	constexpr float PlayerDamageCooldown = 0.80f;

	constexpr float ProjectileRadius = 14.0f;

	constexpr int MaximumPlayerHealth = 5;
	constexpr int MaximumBossHealth = 10;
}

//----------------------

void PlatformerBossFight::start(
	const sf::Vector2u& windowSize)
{
	arenaSize = windowSize;
	jumpKeyWasPressed = false;
	attackKeyWasPressed = false;
	active = true;
	result = PlatformerBossResult::Running;

	playerHealth = MaximumPlayerHealth;
	bossHealth = MaximumBossHealth;

	playerVelocity = { 0.0f, 0.0f };
	bossVelocity = { BossSpeed, 0.0f };

	bossProjectiles.clear();

	projectileTimer = 0.0f;
	trajectoryTimer = 0.0f;

	playerAttackCooldown = 0.0f;
	playerDamageCooldown = 0.0f;

	attackPressed = false;
	playerOnGround = true;

	nextAttackIsSine = true;
	showingTrajectory = false;

	ground.setSize({
		static_cast<float>(arenaSize.x),
		60.0f
		});

	ground.setPosition({
		0.0f,
		static_cast<float>(arenaSize.y) -
			ground.getSize().y
		});

	ground.setFillColor(sf::Color(80, 80, 80));

	player.setSize({
		40.0f,
		60.0f
		});

	player.setPosition({
		80.0f,
		ground.getPosition().y -
			player.getSize().y
		});

	player.setFillColor(sf::Color::Green);

	boss.setSize({
		100.0f,
		120.0f
		});

	boss.setPosition({
		static_cast<float>(arenaSize.x) - 180.0f,
		ground.getPosition().y -
			boss.getSize().y
		});
	playerAttackShape.setSize({
	AttackWidth,
	player.getSize().y - 20.0f
		});

	playerAttackShape.setFillColor(
		sf::Color(255, 255, 100, 170)
	);

	playerAttackVisible = false;
	playerAttackTimer = 0.0f;
	attackPressed = false;
	boss.setFillColor(sf::Color::Red);

	std::cout
		<< "\nThe platformer boss fight begins!\n"
		<< "A/D = Move\n"
		<< "Space = Jump\n"
		<< "K = Attack\n"
		<< "Escape = Leave fight\n";
}

//-------------------------

void PlatformerBossFight::handleEvent(
	const sf::Event& event)
{
	if (!active)
	{
		return;
	}

	const auto* keyPressed =
		event.getIf<sf::Event::KeyPressed>();

	if (keyPressed == nullptr)
	{
		return;
	}

	if (
		keyPressed->code ==
		sf::Keyboard::Key::Escape)
	{
		stop();
	}
}

//----------------------

void PlatformerBossFight::update(float deltaTime)
{
	if (!active)
	{
		return;
	}

	playerAttackCooldown =
		std::max(
			0.0f,
			playerAttackCooldown - deltaTime
		);

	playerDamageCooldown =
		std::max(
			0.0f,
			playerDamageCooldown - deltaTime
		);

	updatePlayer(deltaTime);
	updatePlayerAttack(deltaTime);
	updateBoss(deltaTime);
	updateBossProjectiles(deltaTime);
	handleCollisions();

	if (bossHealth <= 0)
	{
		result = PlatformerBossResult::PlayerWon;
		active = false;

		showingTrajectory = false;
		bossProjectiles.clear();

		std::cout
			<< "The boss was defeated!\n"
			<< "You've been award 5 Capture Orbs!" << std::endl;
	}
	else if (playerHealth <= 0)
	{
		result = PlatformerBossResult::PlayerLost;
		active = false;

		showingTrajectory = false;
		bossProjectiles.clear();

		std::cout
			<< "You were defeated by the boss.\n";
	}
}

//----------------------

void PlatformerBossFight::updatePlayerAttack(
	float deltaTime)
{
	if (!playerAttackVisible)
	{
		return;
	}

	playerAttackTimer += deltaTime;

	if (playerAttackTimer >= AttackVisibleTime)
	{
		playerAttackVisible = false;
		playerAttackTimer = 0.0f;
	}
}

//----------------------

void PlatformerBossFight::updatePlayer(
	float deltaTime)
{
	playerVelocity.x = 0.0f;

	// Horizontal movement.
	if (
		sf::Keyboard::isKeyPressed(
			sf::Keyboard::Key::A))
	{
		playerVelocity.x = -PlayerSpeed;
	}

	if (
		sf::Keyboard::isKeyPressed(
			sf::Keyboard::Key::D))
	{
		playerVelocity.x = PlayerSpeed;
	}

	// Read jump and attack keys every frame.
	const bool jumpKeyPressed =
		sf::Keyboard::isKeyPressed(
			sf::Keyboard::Key::Space);

	const bool attackKeyPressed =
		sf::Keyboard::isKeyPressed(
			sf::Keyboard::Key::K);

	/*
		Jump only when Space changes from released to pressed.
		This prevents holding Space from repeatedly jumping.
	*/
	if (
		jumpKeyPressed &&
		!jumpKeyWasPressed &&
		playerOnGround)
	{
		playerVelocity.y = -JumpSpeed;
		playerOnGround = false;
	}

	/*
		Attack only when K changes from released to pressed.
	*/
	if (
		attackKeyPressed &&
		!attackKeyWasPressed)
	{
		attackPressed = true;
	}

	jumpKeyWasPressed = jumpKeyPressed;
	attackKeyWasPressed = attackKeyPressed;

	// Apply gravity and movement.
	playerVelocity.y += Gravity * deltaTime;

	player.move(playerVelocity * deltaTime);

	sf::Vector2f position =
		player.getPosition();

	const float rightLimit =
		static_cast<float>(arenaSize.x) -
		player.getSize().x;

	position.x = std::clamp(
		position.x,
		0.0f,
		rightLimit
	);

	player.setPosition(position);
}

//----------------------

void PlatformerBossFight::updateBoss(
	float deltaTime)
{
	boss.move(bossVelocity * deltaTime);

	const float leftLimit =
		static_cast<float>(arenaSize.x) * 0.5f;

	const float rightLimit =
		static_cast<float>(arenaSize.x) -
		boss.getSize().x;

	if (boss.getPosition().x <= leftLimit)
	{
		boss.setPosition({
			leftLimit,
			boss.getPosition().y
			});

		bossVelocity.x =
			std::abs(bossVelocity.x);
	}
	else if (
		boss.getPosition().x >= rightLimit)
	{
		boss.setPosition({
			rightLimit,
			boss.getPosition().y
			});

		bossVelocity.x =
			-std::abs(bossVelocity.x);
	}

	// Normal waiting period before the warning begins.
	if (!showingTrajectory)
	{
		projectileTimer += deltaTime;

		if (projectileTimer >= ProjectileInterval)
		{
			projectileTimer = 0.0f;
			trajectoryTimer = 0.0f;
			showingTrajectory = true;
		}

		return;
	}

	// Keep the warning visible before firing.
	trajectoryTimer += deltaTime;

	if (trajectoryTimer < TrajectoryWarningTime)
	{
		return;
	}

	showingTrajectory = false;
	trajectoryTimer = 0.0f;

	if (nextAttackIsSine)
	{
		spawnSineProjectile();
	}
	else
	{
		spawnBouncingProjectile();
	}

	nextAttackIsSine = !nextAttackIsSine;
}

//----------------------

void PlatformerBossFight::drawAttackTrajectory(
	sf::RenderWindow& window) const
{
	if (!showingTrajectory)
	{
		return;
	}

	constexpr int MarkerCount = 24;
	constexpr float PreviewStep = 0.09f;
	constexpr float MarkerRadius = 6.0f;

	sf::CircleShape marker(MarkerRadius);

	marker.setOrigin({
		MarkerRadius,
		MarkerRadius
		});

	marker.setOutlineThickness(1.5f);
	marker.setOutlineColor(sf::Color::White);

	const float flash =
		0.55f +
		0.45f *
		std::sin(trajectoryTimer * 18.0f);

	const std::uint8_t alpha =
		static_cast<std::uint8_t>(
			150.0f + flash * 105.0f
			);

	const float groundTop =
		ground.getPosition().y;

	if (nextAttackIsSine)
	{
		const sf::Vector2f spawnPosition{
			boss.getPosition().x -
				ProjectileRadius * 2.0f,
			boss.getPosition().y +
				boss.getSize().y * 0.45f
		};

		marker.setFillColor(
			sf::Color(255, 255, 80, alpha)
		);

		for (
			int index = 0;
			index < MarkerCount;
			++index)
		{
			const float time =
				static_cast<float>(index) *
				PreviewStep;

			const sf::Vector2f position{
				spawnPosition.x -
					SineProjectileSpeed * time +
					ProjectileRadius,

				spawnPosition.y +
					std::sin(time * 6.0f) *
					50.0f +
					ProjectileRadius
			};

			marker.setPosition(position);
			window.draw(marker);
		}

		return;
	}

	sf::Vector2f position{
		boss.getPosition().x -
			ProjectileRadius * 2.0f +
			ProjectileRadius,

		boss.getPosition().y +
			boss.getSize().y * 0.35f +
			ProjectileRadius
	};

	sf::Vector2f velocity{
		-BounceProjectileSpeed,
		-BounceProjectileJumpSpeed
	};

	marker.setFillColor(
		sf::Color(255, 145, 55, alpha)
	);

	for (
		int index = 0;
		index < MarkerCount;
		++index)
	{
		velocity.y +=
			ProjectileGravity * PreviewStep;

		position += velocity * PreviewStep;

		if (
			position.y + ProjectileRadius >=
			groundTop &&
			velocity.y > 0.0f)
		{
			position.y =
				groundTop - ProjectileRadius;

			velocity.y =
				-BounceProjectileJumpSpeed;
		}

		marker.setPosition(position);
		window.draw(marker);
	}
}

//----------------------

void PlatformerBossFight::spawnSineProjectile()
{
	BossProjectile projectile;

	projectile.type =
		ProjectileType::SineWave;

	projectile.shape =
		sf::CircleShape(ProjectileRadius);

	projectile.shape.setFillColor(
		sf::Color::Yellow
	);

	const sf::Vector2f spawnPosition{
		boss.getPosition().x -
			ProjectileRadius * 2.0f,

		boss.getPosition().y +
			boss.getSize().y * 0.45f
	};

	projectile.shape.setPosition(spawnPosition);

	projectile.velocity = {
		-SineProjectileSpeed,
		0.0f
	};

	projectile.centerY = spawnPosition.y;
	projectile.waveAmplitude = 50.0f;
	projectile.waveFrequency = 6.0f;
	projectile.age = 0.0f;

	bossProjectiles.push_back(projectile);
}

//----------------------

void PlatformerBossFight::spawnBouncingProjectile()
{
	BossProjectile projectile;

	projectile.type =
		ProjectileType::Bouncing;

	projectile.shape =
		sf::CircleShape(ProjectileRadius);

	projectile.shape.setFillColor(
		sf::Color(255, 120, 40)
	);

	projectile.shape.setPosition({
		boss.getPosition().x -
			ProjectileRadius * 2.0f,

		boss.getPosition().y +
			boss.getSize().y * 0.35f
		});

	projectile.velocity = {
		-BounceProjectileSpeed,
		-BounceProjectileJumpSpeed
	};

	projectile.remainingBounces = 4;

	bossProjectiles.push_back(projectile);
}

//----------------------

void PlatformerBossFight::updateBossProjectiles(
	float deltaTime)
{
	const float groundTop =
		ground.getPosition().y;

	for (BossProjectile& projectile :
		bossProjectiles)
	{
		projectile.age += deltaTime;

		if (
			projectile.type ==
			ProjectileType::SineWave)
		{
			sf::Vector2f position =
				projectile.shape.getPosition();

			position.x +=
				projectile.velocity.x * deltaTime;

			position.y =
				projectile.centerY +
				std::sin(
					projectile.age *
					projectile.waveFrequency
				) *
				projectile.waveAmplitude;

			projectile.shape.setPosition(position);
		}
		else
		{
			projectile.velocity.y +=
				ProjectileGravity * deltaTime;

			projectile.shape.move(
				projectile.velocity * deltaTime
			);

			sf::Vector2f position =
				projectile.shape.getPosition();

			const float projectileBottom =
				position.y +
				projectile.shape.getRadius() * 2.0f;

			if (
				projectileBottom >= groundTop &&
				projectile.velocity.y > 0.0f)
			{
				position.y =
					groundTop -
					projectile.shape.getRadius() * 2.0f;

				projectile.shape.setPosition(position);

				projectile.velocity.y =
					-BounceProjectileJumpSpeed;

				--projectile.remainingBounces;
			}
		}
	}

	bossProjectiles.erase(
		std::remove_if(
			bossProjectiles.begin(),
			bossProjectiles.end(),
			[this](const BossProjectile& projectile)
			{
				const sf::Vector2f position =
					projectile.shape.getPosition();

				const bool outsideArena =
					position.x < -80.0f ||
					position.x >
					static_cast<float>(
						arenaSize.x
						) +
					80.0f ||
					position.y < -120.0f ||
					position.y >
					static_cast<float>(
						arenaSize.y
						) +
					120.0f;

				const bool finishedBouncing =
					projectile.type ==
					ProjectileType::Bouncing &&
					projectile.remainingBounces < 0;

				return
					outsideArena ||
					finishedBouncing;
			}
		),
		bossProjectiles.end()
	);
}

//----------------------

void PlatformerBossFight::handleCollisions()
{
	playerOnGround = false;

	const sf::FloatRect playerBounds =
		player.getGlobalBounds();

	const sf::FloatRect groundBounds =
		ground.getGlobalBounds();

	if (
		const std::optional intersection =
		playerBounds.findIntersection(
			groundBounds
		))
	{
		if (
			playerVelocity.y >= 0.0f &&
			intersection->size.y <
			intersection->size.x)
		{
			player.setPosition({
				player.getPosition().x,

				ground.getPosition().y -
					player.getSize().y
				});

			playerVelocity.y = 0.0f;
			playerOnGround = true;
		}
	}

	for (
		auto iterator = bossProjectiles.begin();
		iterator != bossProjectiles.end();)
	{
		if (
			iterator->shape.getGlobalBounds()
			.findIntersection(
				player.getGlobalBounds()
			))
		{
			iterator =
				bossProjectiles.erase(iterator);

			damagePlayer(1);
		}
		else
		{
			++iterator;
		}
	}

	if (attackPressed && playerAttackCooldown <= 0.0f)
	{
		playerAttackCooldown = AttackCooldown;
		playerAttackVisible = true;
		playerAttackTimer = 0.0f;
	}

	if (playerAttackVisible)
	{
		const bool bossIsOnRight =
			boss.getPosition().x + boss.getSize().x * 0.5f >=
			player.getPosition().x + player.getSize().x * 0.5f;

		if (bossIsOnRight)
		{
			playerAttackShape.setPosition({
				player.getPosition().x + player.getSize().x,
				player.getPosition().y + 10.0f
				});
		}
		else
		{
			playerAttackShape.setPosition({
				player.getPosition().x - AttackWidth,
				player.getPosition().y + 10.0f
				});
		}

		if (playerAttackShape.getGlobalBounds().findIntersection(
			boss.getGlobalBounds()))
		{
			damageBoss(1);

			// Prevent hitting every frame while the attack is active.
			playerAttackVisible = false;
			playerAttackTimer = 0.0f;
		}
	}

	attackPressed = false;

	attackPressed = false;
}

//----------------------

void PlatformerBossFight::damagePlayer(
	int damage)
{
	if (playerDamageCooldown > 0.0f)
	{
		return;
	}

	playerDamageCooldown =
		PlayerDamageCooldown;

	playerHealth =
		std::max(
			0,
			playerHealth - damage
		);

	std::cout
		<< "Player health: "
		<< playerHealth
		<< "/"
		<< MaximumPlayerHealth
		<< '\n';
}

//----------------------

void PlatformerBossFight::damageBoss(
	int damage)
{
	bossHealth =
		std::max(
			0,
			bossHealth - damage
		);

	std::cout
		<< "Boss health: "
		<< bossHealth
		<< "/"
		<< MaximumBossHealth
		<< '\n';
}

//----------------------

void PlatformerBossFight::draw(
	sf::RenderWindow& window)
{
	if (!active)
	{
		return;
	}

	window.draw(ground);
	window.draw(player);
	window.draw(boss);

	for (const BossProjectile& projectile :
		bossProjectiles)
	{
		window.draw(projectile.shape);
	}

	// Draw the warning after the characters and projectiles
	// so that it remains visible.
	drawAttackTrajectory(window);

	sf::RectangleShape playerHealthBackground({
		150.0f,
		18.0f
		});

	playerHealthBackground.setPosition({
		18.0f,
		18.0f
		});

	playerHealthBackground.setFillColor(
		sf::Color(45, 45, 45)
	);

	window.draw(playerHealthBackground);

	sf::RectangleShape playerHealthBar({
		150.0f *
			static_cast<float>(playerHealth) /
			static_cast<float>(
				MaximumPlayerHealth
			),
		18.0f
		});

	playerHealthBar.setPosition({
		18.0f,
		18.0f
		});

	playerHealthBar.setFillColor(
		sf::Color::Green
	);

	window.draw(playerHealthBar);

	sf::RectangleShape bossHealthBackground({
		220.0f,
		18.0f
		});

	bossHealthBackground.setPosition({
		static_cast<float>(arenaSize.x) -
			238.0f,
		18.0f
		});

	bossHealthBackground.setFillColor(
		sf::Color(45, 45, 45)
	);

	window.draw(bossHealthBackground);

	sf::RectangleShape bossHealthBar({
		220.0f *
			static_cast<float>(bossHealth) /
			static_cast<float>(
				MaximumBossHealth
			),
		18.0f
		});

	bossHealthBar.setPosition({
		static_cast<float>(arenaSize.x) -
			238.0f,
		18.0f
		});

	bossHealthBar.setFillColor(
		sf::Color::Red
	);

	window.draw(bossHealthBar);
}

bool PlatformerBossFight::isActive() const
{
	return active;
}

PlatformerBossResult
PlatformerBossFight::getResult() const
{
	return result;
}

//----------------------

void PlatformerBossFight::stop()
{
	active = false;
	showingTrajectory = false;

	bossProjectiles.clear();

	result =
		PlatformerBossResult::Cancelled;
}